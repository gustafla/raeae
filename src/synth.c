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

#define ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))

#include "globals.h"
#include <math.h>
#include <string.h>
#include <SDL.h>
#include <limits.h>
#include "song.c"

#ifndef DNLOAD_VIDEOCORE
SDL_AudioDeviceID gSynthAudioDevice;
#endif

/* Note strings will be compiled to a list of indices to the freq table in synthInit() */
/*static signed const gSynthSq1Notes[sizeof(songsq1)/sizeof(songsq1[0])];*/

/* Note encoding copied from YROT, why change something that works? ;D */
static char const *gSynthNotes[] = {
    "c1", "C1", "d1", "D1", "e1", "f1", "F1", "g1", "G1", "a1", "A1", "b1",
    "c2", "C2", "d2", "D2", "e2", "f2", "F2", "g2", "G2", "a2", "A2", "b2",
    "c3", "C3", "d3", "D3", "e3", "f3", "F3", "g3", "G3", "a3", "A3", "b3",
    "c4", "C4", "d4", "D4", "e4", "f4", "F4", "g4", "G4", "a4", "A4", "b4",
    "c5", "C5", "d5", "D5", "e5", "f5", "F5", "g5", "G5", "a5", "A5", "b5",
    "c6", "C6", "d6", "D6", "e6", "f6", "F6", "g6", "G6", "a6", "A6", "b6",
    "0"
};

static float gSynthFreqs[(sizeof(gSynthNotes) / sizeof(gSynthNotes[0])) - 1];

static int const      G_SYNTH_BASE_NOTE         = 12*3+10; /* n(notes) * n(octavesUndera4) + n(notesUndera4) */
static unsigned const G_SYNTH_PEAK              = 0x7FFF; /*int16_t*/
static float const    G_SYNTH_TUNE              = 440.f;

static unsigned const G_SYNTH_AUDIO_RATE        = 44100;
static unsigned const G_SYNTH_AUDIO_CHANNELS    = 2;
static unsigned const G_SYNTH_AUDIO_DEPTH       = 2;

static unsigned const G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT = (unsigned)(G_DEMO_LENGTH/G_DEMO_TIMESCALE) * G_SYNTH_AUDIO_RATE;
static unsigned const G_SYNTH_AUDIO_STREAM_SIZE = (
    G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH
);

/* Runtime precalc'd audio stram */
uint8_t gSynthAudioStream[G_SYNTH_AUDIO_STREAM_SIZE];

/* Runtime buffer playback variables */
static struct {
    uint32_t len = G_SYNTH_AUDIO_STREAM_SIZE;
    uint8_t *pos = gSynthAudioStream;
} gSynthPlayback;

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

/* Oscillators */
int16_t synthOscSquare(unsigned pos, float freq) {
    /* Period time in samples */
    unsigned period = (unsigned)((float)G_SYNTH_AUDIO_RATE/freq);
    return (pos % period > period / 2 ? G_SYNTH_PEAK : -G_SYNTH_PEAK);
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
unsigned synthSeq(unsigned pos) {
    /* Work out which beat pos is at */
    float const bps = songbpm/60.f;
    float const beatTime = 1.f/bps;
    unsigned beatTimeSamples = (unsigned)(beatTime * (float)G_SYNTH_AUDIO_RATE);
    unsigned beatNum = pos/beatTimeSamples;

    /* Limit beat num to known sane values and return it, rest is done by mixer */
    return (beatNum < 0 ? 0 : beatNum);
}

int16_t synthMix(unsigned pos /* in samples */) {
    unsigned seqPos = synthSeq(pos);

    unsigned sq1Len = ARRAY_LEN(songsq1);
    return synthOscSquare(pos, synthNoteFreq(songsq1[seqPos > sq1Len-1 ? sq1Len-1 : seqPos]));
}

void synthPlay16(uint8_t *buf, unsigned pos /* Position in samples */) {
    int16_t l, r;
    
    /* Get audio... */
    r = l = synthMix(pos);

    /* Slice the samples into the buffer */
    buf[pos * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH]     = l         & 0xff;
    buf[pos * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH + 1] = (l >> 8)  & 0xff;
    buf[pos * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH + 2] = r         & 0xff;
    buf[pos * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH + 3] = (r >> 8)  & 0xff;
}


void synthInit() {
    unsigned pos; /* Position in samples */
    int i;

    /* Calculate note frequencies */
    for (i=0; i<ARRAY_LEN(gSynthFreqs); i++) {
        /* http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html */
        /* Add 1 because 0 is array index origin */
        gSynthFreqs[i] = G_SYNTH_TUNE * dnload_powf(dnload_powf(2.f, 1.f/12.f), (float)(i-G_SYNTH_BASE_NOTE+1));
    }

    /* Compile song notes to freq indices for all channels */
    /*for (i=0; i<(sizeof(songsq1)/sizeof(songsq1[0])); i++) {
        gSynthSq1Notes[i] = noteIndex(songsq1[i]);
    }*/
    /* Add more channels here... */

    /* Calculate song PCM buffer */
    for (pos=0; pos < G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT; pos++) {
        synthPlay16(gSynthAudioStream, pos);
    }

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

#endif
