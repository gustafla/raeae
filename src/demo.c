#include "dnload.h"
#include "globals.h"
#include "video.c"
#include <stdio.h>

#ifndef DEMO_C
#define DEMO_C

void demoMainLoop() {
#ifdef USE_LD
    /* FPS counter vars */
    unsigned fpsTimePrint = 0;
    unsigned fpsFrames = 0;
    unsigned const FPS_TIME = 2000;
#endif

    dnload_glClearColor(0,0,0,1);
    while (gDemoRunning) {
        gCurTime = dnload_SDL_GetTicks();
        dnload_glClear(GL_COLOR_BUFFER_BIT);
        videoSwapBuffers();

#ifdef USE_LD
        /* Count and print FPS */
        if (fpsTimePrint + FPS_TIME > gCurTime) {
            printf("FPS: %f\n", (float)fpsFrames/((float)FPS_TIME/1000.0f));
            fpsTimePrint = gCurTime;
            fpsFrames=0;
        } else {
            fpsFrames++;
        }
#endif
    }
}

#endif
