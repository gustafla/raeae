#ifndef SYNTH_C
#define SYNTH_C

#include "globals.h"
#include <math.h>
#include <string.h>
#include <SDL.h>
#include <limits.h>

#ifndef DNLOAD_VIDEOCORE
SDL_AudioDeviceID gSynthAudioDevice;
#endif

static unsigned const G_SYNTH_PEAK              = 0xFFFF;

static unsigned const G_SYNTH_AUDIO_RATE        = 44100;
static unsigned const G_SYNTH_AUDIO_CHANNELS    = 2;
static unsigned const G_SYNTH_AUDIO_DEPTH       = 2;

static unsigned const G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT = (G_DEMO_LENGTH/1000) * G_SYNTH_AUDIO_RATE;
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

void synthStreamCallback(void *userdata, uint8_t *stream, int len) {
    unsigned i, offset;
    /* Don't play if empty */
    if (gSynthPlayback.len == 0)
        return;

    /* Don't read over the buffer */
    len = (len > gSynthPlayback.len ? gSynthPlayback.len : len);

    /* All data is premixed, just copy */
    /*dnload_memcpy((void*)stream, (void*)gSynthPlayback.pos, len); */
    for (i=0; i<len; i++) {
        stream[i] = gSynthPlayback.pos[i];
    }

    /* Move to next samples */
    gSynthPlayback.pos += len;

    /* Keep track of data left available in buffer */
    gSynthPlayback.len -= len;
}

void synthPlay16(uint8_t *buf, unsigned pos /* Position in samples */) {
    uint16_t l, r;
    unsigned period;
    float t = (float)pos/(float)G_SYNTH_AUDIO_RATE;
    
    /* Gen test audio... */
    period = G_SYNTH_AUDIO_RATE/440;
    l = ((pos % period) > (period/2) ? G_SYNTH_PEAK : 0);
    r = ((pos % period) > (period/2) ? G_SYNTH_PEAK : 0);

    /* Slice the samples into the buffer */
    buf[pos]    = l         & 0xff;
    buf[pos+1]  = (l >> 8)  & 0xff;
    buf[pos+2]  = r         & 0xff;
    buf[pos+3]  = (r >> 8)  & 0xff;
}

void synthInitSDL16() {
    /* These are hardcoded to the synth... */
    SDL_AudioSpec want, have;
    /*dnload_SDL_memset(&want, 0, sizeof(want));*/
    want.freq = G_SYNTH_AUDIO_RATE;
    want.format = AUDIO_U16LSB;
    want.channels = G_SYNTH_AUDIO_CHANNELS;
    want.samples = 4096;
    want.callback = synthStreamCallback;

#ifdef DNLOAD_VIDEOCORE
    SDL_OpenAudio(&want, NULL);
#else
    gSynthAudioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
#endif
}

void synthInit() {
    /* Position in samples */
    unsigned pos;
    for (pos=0; pos < G_SYNTH_AUDIO_STREAM_SAMPLE_COUNT; pos++) {
        synthPlay16(gSynthAudioStream, pos);
    }

#ifdef USE_LD
    puts("Synth render done.");
#endif

    synthInitSDL16();
}

void synthStartStream() {
#ifdef DNLOAD_VIDEOCORE
    SDL_PauseAudio(0);
#else
    SDL_PauseAudioDevice(gSynthAudioDevice, 0);
#endif
}

#endif
