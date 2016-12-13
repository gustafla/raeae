#ifndef VIDEO_C
#define VIDEO_C

#include "dnload.h"

#ifdef DNLOAD_VIDEOCORE
#include "dnload_egl.h"
#include "dnload_videocore.h"
#endif

#include <GLES2/gl2.h>

/* Covers both SDLs, makefile decides Iflag */
#include <SDL.h>


#ifndef DNLOAD_VIDEOCORE
static SDL_Window *gVideoSDLWindow;
#endif

void videoInit(int w, int h) {
#ifdef DNLOAD_VIDEOCORE
    videocore_create_native_window(w, h);
    egl_init((NativeWindowType)&g_egl_native_window, &g_egl_display, &g_egl_surface);
    /* Needed for keyb */
    dnload_SDL_SetVideoMode(0, 0, 0, SDL_HWSURFACE);
#else
    dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    gVideoSDLWindow = dnload_SDL_CreateWindow(NULL, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL);
    dnload_SDL_GL_CreateContext(gVideoSDLWindow);
#endif
    dnload_SDL_ShowCursor(0);
}

void videoSwapBuffers() {
#ifdef DNLOAD_VIDEOCORE
    dnload_eglSwapBuffers(g_egl_display, g_egl_surface);
#else
    dnload_SDL_GL_SwapWindow(gVideoSDLWindow);
#endif
}

void videoDeinit() {
#ifdef DNLOAD_VIDEOCORE
    egl_quit(g_egl_display);
    dnload_bcm_host_deinit();
#endif
    dnload_SDL_Quit();
}

#endif /* VIDEO_C */
