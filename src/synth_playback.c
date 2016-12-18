#ifndef SYNTH_PLAYBACK_C
#define SYNTH_PLAYBACK_C

#include "synth.h"

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

#endif /* SYNTH_PLAYBACK_C */
