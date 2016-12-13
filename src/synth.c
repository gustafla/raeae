/* Design notes:
    
    arbitrary number of instruments that have a waveform and ADSR parameters
    note progressions, or "patterns" are arbitrary as well, have some sym for no change
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
unsigned synthNoteIndex(char const *note) {
    unsigned index;
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

/* Return note index from notes that corresponds to pos and tempo */
unsigned synthSeq(float t) {
    /* Work out which beat pos is at */
    float const bps = songbpm/60.f; /* SONGBPM REF CHANGEME */
    float const beatTime = 1.f/bps;
    unsigned beatNum = (unsigned)(t/beatTime); /* Relying on float->int truncation */

    /* Limit beat num to known sane values and return it, rest is done by mixer */
    return (beatNum < 0 ? 0 : beatNum);
}

float synthMix(float t) { /* CHANGEME */
    unsigned seqPos = synthSeq(t);

    unsigned sq1Len = ARRAY_LEN(songsq1);
    return synthOscSquare(t, synthNoteFreq(songsq1[seqPos > sq1Len-1 ? sq1Len-1 : seqPos]));
}

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


void synthInit() {
    unsigned pos; /* Position in samples */
    unsigned const PRINT_BLK = 4096;
    int i;

    /* Calculate note frequencies */
    for (i=0; i<ARRAY_LEN(gSynthFreqs); i++) {
        /* http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html */
        /* Add 1 because 0 is array index origin */
        gSynthFreqs[i] = G_SYNTH_TUNE * dnload_powf(1.05946309f, (float)(i-G_SYNTH_BASE_NOTE+1));
    }

    /* Compile song notes to freq indices for all channels */
    /*for (i=0; i<(sizeof(songsq1)/sizeof(songsq1[0])); i++) {
        gSynthSq1Notes[i] = noteIndex(songsq1[i]);
    }*/
    /* Add more channels here... */

    /* Calculate song PCM buffer */
    for (pos=0; pos < G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT; pos++) {
        synthPlay16(gSynthAudioStream, pos);

        /* Rudimentary progress indicator */
        if (pos % PRINT_BLK == 0) {
            dnload_printf("\rMusic precalc: %i%%", (int)(100.f * ((float)pos / (float)G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT)));
        }
    }
    /* Change line from progress indicator */
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
