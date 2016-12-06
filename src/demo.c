#include "globals.h"
#include "video.c"

#ifndef DEMO_C
#define DEMO_C

void demoMainLoop() {
    dnload_glClearColor(0,0,0,1);
    while (gDemoRunning) {
        dnload_glClear(GL_COLOR_BUFFER_BIT);
        videoSwapBuffers();
    }
}

#endif
