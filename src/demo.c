#ifndef DEMO_C
#define DEMO_C

#include "dnload.h"
#include "globals.h"
#include "video.c"
#include <stdio.h>

void drawQuad(GLint program) {
    float quad[2*3*3] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        1.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0
    };
    float qtex[2*3*2] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0
    };

    dnload_glUseProgram(program);

    dnload_glBindAttribLocation(program, 0, "a_pos");
    dnload_glBindAttribLocation(program, 1, "a_texcoord");

    dnload_glEnableVertexAttribArray(0);
    dnload_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, quad);
    dnload_glEnableVertexAttribArray(1);
    dnload_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, qtex);

    dnload_glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLint loadShader(const char* src, GLenum type) {
    GLint handle = dnload_glCreateShader(type);
    dnload_glShaderSource(handle, 1, &src, NULL);
    dnload_glCompileShader(handle);

#ifdef USE_LD
    GLint compiled;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE) {
        glDeleteShader(handle);
        return -1;
    }
#endif

    return handle;
}

GLint linkProgram(GLint a, GLint b) {
    GLint handle = dnload_glCreateProgram();
    dnload_glUseProgram(handle);

    dnload_glAttachShader(handle, a);
    dnload_glAttachShader(handle, b);
    dnload_glLinkProgram(handle);

#ifdef USE_LD
    GLint linked;
    glGetProgramiv(handle, GL_LINK_STATUS, &linked);

    if (linked == GL_FALSE) {
        glDeleteProgram(handle);
        return -1;
    }
#endif

    return handle;
}

void demoMainLoop(unsigned start) {
    unsigned realTime=0;

    /* Synth should've been initialized already, use song bpm to scale demo speed */
    float const TIMESCALE = (float)gSynthSongData.bpm / 60.f;

#ifdef USE_LD
    /* FPS counter vars */
    unsigned fpsTimePrint = 0;
    unsigned fpsFrames = 0;
    unsigned const FPS_TIME = 2000;
#endif

    SDL_Event event;

    dnload_glClearColor(1,0,0,1);
    dnload_glViewport(0, 0, G_VIDEO_X, G_VIDEO_Y);
    dnload_glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /****************************************************************************/

    static const char* vertShader = "attribute vec3 a_pos;\n"
                       "attribute vec2 a_texcoord;\n"
                       "varying vec2 v_texcoord;\n"
                       "void main() {\n"
                       "  v_texcoord = a_texcoord;\n"
                       "  gl_Position = vec4(a_pos, 1.);\n"
                       "}\n";

    static const char* fragShader = "varying vec2 v_texcoord;\n"
                       "uniform float u_time;\n"
                       "uniform float u_beat;\n"
                       "void main() {\n"
                       "  gl_FragColor = vec4(0., 1., 0., 1.);\n"
                       "}\n";
    
    const GLint shader = linkProgram(loadShader(vertShader, GL_VERTEX_SHADER), loadShader(fragShader, GL_FRAGMENT_SHADER));

    /***************************************************************************/

    /* Ready to rock and roll, start the music now */
    synthStartStream();

    while (1) {
        /* realTime is local and contains time in msec */
        realTime = dnload_SDL_GetTicks()-start;
        /* gCurTime is global and contains time in musical beats */
        gCurTime = ((float)realTime / 1000.f) * TIMESCALE;

        dnload_SDL_PollEvent(&event);
        /* End demo if current time in secs is more than demo length in secs */
        if (event.type == SDL_KEYDOWN | event.type == SDL_QUIT | gCurTime/TIMESCALE >G_DEMO_LENGTH) {
            break;
        }

        dnload_glClear(GL_COLOR_BUFFER_BIT);

        drawQuad(shader);

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
