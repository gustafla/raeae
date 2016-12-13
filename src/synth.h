#ifndef SYNTH_H
#define SYNTH_H

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

#endif /* SYNTH_H */
