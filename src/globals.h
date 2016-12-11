#ifndef GLOBALS_H
#define GLOBALS_H

#include "song.c"

static int              gDemoRunning = 1;
static int const        G_VIDEO_X = 1280;
static int const        G_VIDEO_Y = 720;
static float            gCurTime = 0;
/* Try to have all times in beat counts */
static float const      G_DEMO_LENGTH = 60.f;
/* For gCurTime */
static float const      G_DEMO_TIMESCALE = songbpm / 60.f;
/* Because M_PI isn't standard */
static float const      G_PI = 3.14159265;

#endif
