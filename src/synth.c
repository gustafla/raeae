#ifndef SYNTH_C
#define SYNTH_C

#include "globals.h"
#include <SDL.h>

static unsigned const G_SYNTH_AUDIO_RATE        = 44100;
static unsigned const G_SYNTH_AUDIO_CHANNELS    = 2;
static unsigned const G_SYNTH_AUDIO_DEPTH       = 2;

static unsigned const G_SYNTH_AUDIO_STREAM_SIZE = (
    (G_DEMO_LENGTH/1000) * G_SYNTH_AUDIO_RATE * G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH
);

/* Runtime precalc'd audio stram */
uint8_t gSynthAudioStream[G_SYNTH_AUDIO_STREAM_SIZE];

/* Runtime buffer playback variables */
static struct {
    uint32_t len = G_SYNTH_AUDIO_STREAM_SIZE;
    uint8_t *pos = &gSynthAudioStream;
} gSynthPlayback;

void synthCallback(uint8_t *stream, int len) {
    /* Don't play if empty */
    if (gSynthPlayback.len == 0)
        return;

    /* Don't read over the buffer */
    len = (len > gSynthPlayback.len ? gSynthPlayback.len : len);

    /* All data is premixed, just copy */
    dnload_SDL_memcpy(stream, gSynthPlayback.pos, len);

    /* Move to next samples */
    gSynthPlayback.pos += len;

    /* Keep track of data left available in buffer */
    gSynthPlayback.len -= len;
}

void synthPlay(uint8_t *buf, unsigned pos, unsigned len) {
    
}

void synthInit() {
    /* Position in bytes */
    unsigned pos;
    /* Size of a sample */
    unsigned const SAMPLE_LEN = G_SYNTH_AUDIO_CHANNELS * G_SYNTH_AUDIO_DEPTH;
    for (pos=0; pos<G_SYNTH_AUDIO_STREAM_SIZE, pos+=SAMPLE_LEN) {
        synthPlay(gSynthAudioStream, );
    }
}

#endif
