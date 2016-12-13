#ifndef DEMO_C
#define DEMO_C

#include "dnload.h"
#include "globals.h"
#include "video.c"
#include <stdio.h>

void demoMainLoop(unsigned start) {
    unsigned realTime=0;
#ifdef USE_LD
    /* FPS counter vars */
    unsigned fpsTimePrint = 0;
    unsigned fpsFrames = 0;
    unsigned const FPS_TIME = 2000;
#endif

    SDL_Event event;

    dnload_glClearColor(1,0,0,1);

    /* Ready to rock and roll, start the music now */
    synthStartStream();

    while (gDemoRunning) {
        /* realTime is local and contains time in msec */
        realTime = dnload_SDL_GetTicks()-start;
        /* gCurTime is global and contains time in musical beats */
        gCurTime = ((float)realTime / 1000.f) * G_DEMO_TIMESCALE;

        dnload_SDL_PollEvent(&event);
        if (event.type == SDL_KEYDOWN | event.type == SDL_QUIT | gCurTime>G_DEMO_LENGTH)
            gDemoRunning = 0;

        dnload_glClear(GL_COLOR_BUFFER_BIT);
        videoSwapBuffers();

#ifdef USE_LD
        /* Count and print FPS */
        if (fpsTimePrint + FPS_TIME < realTime) {
            printf("FPS: %f\n", (float)fpsFrames/((float)FPS_TIME/1000.0f));
            fpsTimePrint = realTime;
            fpsFrames=0;
        } else {
            fpsFrames++;
        }
#endif
    }
}

#endif /* DEMO_C */
