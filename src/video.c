#include "dnload.h"

#include "dnload_egl.h"
#include "dnload_videocore.h"
#include <GLES2/gl2.h>
#include <SDL.h>

void videoInit(int w, int h) {
    videocore_create_native_window(w, h);
}

void videoSwapBuffers() {
    dnload_eglSwapBuffers(g_egl_display, g_egl_surface);
}

void videoDeinit() {
    egl_quit(g_egl_display);
    dnload_bcm_host_deinit();
}
