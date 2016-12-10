#ifndef GLOBALS_H
#define GLOBALS_H

static int              gDemoRunning = 1;
static int const        G_VIDEO_X = 1280;
static int const        G_VIDEO_Y = 720;
static unsigned         gCurTime = 0;
/* Trying to have all times in msec because of SDL */
static unsigned const   G_DEMO_LENGTH = 60*1000;

#endif
