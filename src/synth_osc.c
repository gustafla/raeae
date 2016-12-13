#ifndef SYNTH_OSC_C
#define SYNTH_OSC_C

#include "synth.h"
#include "math.h"

/* Oscillators produce floating point audio from 0 to 1 */

/* Square wave */
float synthOscSquare(float t, float f) {
    /* Period time */
    float pt = 1.f / f;
    return (dnload_fmodf(t, pt) > pt / 2.f ? 1.f : 0.f);
}

#endif /* SYNTH_OSC_C */
