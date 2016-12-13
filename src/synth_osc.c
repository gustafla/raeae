#ifndef SYNTH_OSC_C
#define SYNTH_OSC_C

/* Oscillators */
int16_t synthOscSquare(unsigned pos, float freq) {
    /* Period time in samples */
    unsigned period = (unsigned)((float)G_SYNTH_AUDIO_RATE/freq);
    return (pos % period > period / 2 ? G_SYNTH_PEAK : -G_SYNTH_PEAK);
}

#endif /* SYNTH_OSC_C */
