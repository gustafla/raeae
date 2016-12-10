#include "dnload.h"
#include "globals.h"
#include "video.c"
#include <stdio.h>

#ifndef DEMO_C
#define DEMO_C

void demoMainLoop(unsigned start) {
#ifdef USE_LD
    /* FPS counter vars */
    unsigned fpsTimePrint = 0;
    unsigned fpsFrames = 0;
    unsigned const FPS_TIME = 2000;
#endif

    SDL_Event event;

    dnload_glClearColor(0,0,0,1);

    /* Ready to rock and roll, start the music now */
    synthStartStream();

    while (gDemoRunning) {
        dnload_SDL_PollEvent(&event);
        if (event.type == SDL_KEYDOWN | event.type == SDL_QUIT)
            gDemoRunning = 0;

        gCurTime = dnload_SDL_GetTicks()-start;
        dnload_glClear(GL_COLOR_BUFFER_BIT);
        videoSwapBuffers();

#ifdef USE_LD
        /* Count and print FPS */
        if (fpsTimePrint + FPS_TIME < gCurTime) {
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
