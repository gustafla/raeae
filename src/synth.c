/* Design notes:
    
    every track has an instrument that has a waveform and ADSR parameters
    note progressions, or "patterns" arbitrary in number and length, have some sym for no change
    tracks or channels just list pattern nums arbitrarily
    have all song data in a string and process it using strtok and atoi, basically copy YROT
    keep audio floating point until final mixdown where converted to int16_t
    JUST BASICALLY COPY YROT XD

*/

#ifndef SYNTH_C
#define SYNTH_C

#include "synth.h"
#include "synth_osc.c"

/* Buffer fill callback for SDL */
void synthStreamCallback(void *userdata, uint8_t *stream, int len) {
    unsigned i, offset;
    /* Don't play if empty */
    if (gSynthPlayback.len == 0)
        return;

    /* Don't read over the buffer */
    len = (len > gSynthPlayback.len ? gSynthPlayback.len : len);

    /* All data is premixed, just copy */
    dnload_memcpy((void*)stream, (void*)gSynthPlayback.pos, len);
    /*for (i=0; i<len; i++) {
        stream[i] = gSynthPlayback.pos[i];
    }*/

    /* Move to next samples */
    gSynthPlayback.pos += len;

    /* Keep track of data left available in buffer */
    gSynthPlayback.len -= len;
}

/* Open SDL audio playback */
void synthInitSDL16() {
    /* These are hardcoded to the synth... */
    SDL_AudioSpec want, have;
    /*dnload_SDL_memset(&want, 0, sizeof(want));*/
    want.freq = G_SYNTH_AUDIO_RATE;
    want.format = AUDIO_S16LSB;
    want.channels = G_SYNTH_AUDIO_CHANNELS;
    want.samples = 4096;
    want.callback = synthStreamCallback;

#ifdef DNLOAD_VIDEOCORE
    dnload_SDL_OpenAudio(&want, NULL);
#else
    gSynthAudioDevice = dnload_SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
#ifdef USE_LD
    if (want.format != have.format) {
        puts("Failed to get S16LSB audio");
    }
#endif
#endif
}

/* Find from note string */
int synthNoteIndex(char const *note) { /* CHANGEME REWRITE USING KNOWN NOTE TABLE SIZE */
    int index;
    /* I know hashing would maybe be faster but this is fine for precalc synth PoC */
    for (index = 0; dnload_strcmp(note, gSynthNotes[index]) != 0; index++) {
        /* If no match found and end of note table, "0", is matched set index to a sane value and give up */
        if (dnload_strcmp("0", gSynthNotes[index]) == 0) {
            index=0;
            break;
        }
    }

    return index;
}

float synthNoteFreq(char const *note) {
    /* These should be calcd in synthInit() */
    return gSynthFreqs[synthNoteIndex(note)];
}

/* Return beat num from notes that corresponds to pos and tempo */
int synthSeq(float t) {
    /* Work out which beat pos is at */
    float const beatTime = 60.f/gSynthSongData.bpm;
    int beatNum = (int)(t/beatTime); /* Relying on float->int truncation */

    /* Limit beat num to known sane values and return it, rest is done by mixer */
    return (beatNum < 0 ? 0 : beatNum);
}

float synthMix(float t) {
    int seqPos = synthSeq(t);
    int trackNum, pattIndex, pattPos, note;
    /* Keeps track of where each track is currently */
    static int trackPos[G_SYNTH_MAX_TRACKS];
    float sample = 0.f;

    for (trackNum = 0; trackNum < G_SYNTH_MAX_TRACKS; trackNum++) {
        /* This is the pattern that we're supposed to be playing from now */
        pattIndex = gSynthSongData.tracks[trackNum][trackPos[trackNum]];
        if (pattIndex == END) { /* However, if the track is returning end instead, it's done playing for good */
            continue;           /* and no further operations are necessary */
        }
        if (pattIndex == LOOP) { /* We also might find that the track wants to loop back to beginning */
            trackPos[trackNum] = 0; /* So we set the track to beginning! */
            pattIndex = gSynthSongData.tracks[trackNum][0]; /* And update the currently wanted pattern too */
        }
        
        /* Now we know what pattern the track wants to play, so let's work out which note the pattern is at */
        note = gSynthSongData.patterns[pattIndex][seqPos/* - loops - pc*/];
        if (note == END) { /* The note might just be that the pattern is at end and has no more notes to play */
            trackPos[trackNum]++; /* Move on to next pattern in the track! */
        } else { /* Actually, fuck this, let's unencode the tracks in synthInit to notes and then just linearly play from it, checking if it loops or not */
            sample += gSynthSongData.instruments[trackNum].waveform(t, gSynthFreqs[note]) * 
                      gSynthSongData.instruments[trackNum].volume;
        }
    }

    return sample;
}

/* "render" sample number pos, both channels, to int16 format */
void synthPlay16(uint8_t *buf, unsigned pos /* Position in samples */) {
    int16_t lS16, rS16;
    float l, r;
    
    /* Get audio... */
    r = l = synthMix((float)pos / (float)G_SYNTH_AUDIO_RATE); /* Convert sample pos to time */

    /* Convert floating point to S16 */
    lS16 = (unsigned)(l * (float)G_SYNTH_PEAK);
    rS16 = (unsigned)(r * (float)G_SYNTH_PEAK);

    /* Slice the samples into the buffer */
    buf[pos * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH]     = lS16         & 0xff;
    buf[pos * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH + 1] = (lS16 >> 8)  & 0xff;
    buf[pos * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH + 2] = rS16         & 0xff;
    buf[pos * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH + 3] = (rS16 >> 8)  & 0xff;
}

/* Fills gSynthSongData from song file */
void synthLoadSong(char *song) {
    /* num lines, cur line, key with number */
    int n, i, j, nk;
    /* song file separated, tmp key from line (word before ':'), tmp word such as waveform */
    char *lines[G_SYNTH_MAX_SONG_FILE_LINES], *key, *tmp;

    /* Separate lines by \n */
    /* Recommended reading: some online strtok reference */
    for (n=0; n<G_SYNTH_MAX_SONG_FILE_LINES; n++) {
        lines[n] = dnload_strtok((n) ? NULL : song, "\n");
        if (lines[n] == NULL) {
            break;
        }
    }

    /* Extract data from line */
    for (i=0; i<n; i++) {
        /* lines starting with '#' are comments */
        if (lines[i][0] == '#') {
            continue;
        }

        /* check line key and react accordingly */
        if (key = dnload_strtok(lines[i], ":")) {
            /* Song BPM */
            if (dnload_strcmp(key, "bpm") == 0) {
                gSynthSongData.bpm = dnload_atoi(dnload_strtok(NULL, ":"));
#ifdef USE_LD
                printf("Found song BPM: %i\n", gSynthSongData.bpm);
#endif
            }
            /* Instrument :(nk) */
            if (key[0] == 'i') {
                nk = dnload_atoi(&key[1]);
                
                tmp = dnload_strtok(NULL, ",");
                if (dnload_strcmp(tmp, "sq") == 0) {
                    gSynthSongData.instruments[nk].waveform = synthOscSquare;
                } /* Add more waveforms here... */

                gSynthSongData.instruments[nk].volume = (float)dnload_atoi(dnload_strtok(NULL, ",")) / 100.f;
#ifdef USE_LD
                printf("Found instrument %i: waveform %s, volume %f\n", nk, tmp, gSynthSongData.instruments[nk].volume);
#endif
            }
            /* Pattern (:nk) */
            if (key[0] == 'p') {
                nk = dnload_atoi(&key[1]);

                j = 0;
                while(1) {
                    tmp = dnload_strtok(NULL, ",");
                    if (tmp != NULL) {
                        gSynthSongData.patterns[nk][j] = synthNoteIndex(tmp);
                        j++;
                    } else {
                        gSynthSongData.patterns[nk][j] = END;
                        break;
                    }
                }
#ifdef USE_LD
                printf("Found pattern %i: length %i\n", nk, j);
#endif
            }
            /* Track (:nk) */
            if (key[0] == 't') {
                nk = dnload_atoi(&key[1]);

                j = 0;
                while(1) {
                    tmp = dnload_strtok(NULL, ",");
                    if (tmp != NULL) {
                        if (dnload_strcmp(tmp, "loop") == 0) {
                            gSynthSongData.tracks[nk][j] = LOOP;
                        } else {
                            gSynthSongData.tracks[nk][j] = dnload_atoi(tmp);
                        }
                        j++;
                    } else {
                        gSynthSongData.tracks[nk][j] = END;
                        break;
                    }
                }
#ifdef USE_LD
                printf("Found track %i: length %i\n", nk, j);
#endif
            }
        }
    }
}

void synthInit() {
    unsigned pos; /* Position in samples */
    unsigned const PRINT_BLK = 4096;
    int i;

    for (i=0; i<G_SYNTH_MAX_PATTERNS; i++) {
        gSynthSongData.patterns[i][0] = END;
    }

    for (i=0; i<G_SYNTH_MAX_TRACKS; i++) {
        gSynthSongData.tracks[i][0] = END;
    }

    synthLoadSong(gSongFile);

    /* CHANGEME rewrite counting to a4 and using that as SYNTH_BASE_NOTE instead */
    /* Calculate note frequencies */
    for (i=0; i<ARRAY_LEN(gSynthFreqs); i++) {
        /* http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html */
        /* Add 1 because 0 is array index origin */
        gSynthFreqs[i] = G_SYNTH_TUNE * dnload_powf(1.05946309f, (float)(i-G_SYNTH_BASE_NOTE+1));
    }

    /* Calculate song PCM buffer */
    for (pos=0; pos < G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT; pos++) {
        synthPlay16(gSynthAudioStream, pos);

        /* Rudimentary progress indicator */
        if (pos % PRINT_BLK == 0) {
            dnload_printf("\rMusic precalc: %i%%", (int)(100.f * ((float)pos / (float)G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT)));
        }
    }
    /* Print a new line that isn't the progress indicator */
    dnload_printf("\n");

#ifdef USE_LD
    puts("Synth render done.");
    printf("Synth c4 is: %f\n", synthNoteFreq("c4"));
#endif

    synthInitSDL16();
}

void synthStartStream() {
#ifdef DNLOAD_VIDEOCORE
    dnload_SDL_PauseAudio(0);
#else
    dnload_SDL_PauseAudioDevice(gSynthAudioDevice, 0);
#endif
}

#endif /* SYNTH_C */
