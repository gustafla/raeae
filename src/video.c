#include <bcm_host.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifndef VIDEO_C
#define VIDEO_C

static EGLDisplay display;
static EGLSurface surface;
static EGLContext context;

static uint32_t width,height;

/* Kinda stolen from YROT but does it make a
   difference when I could've written this manually too...*/
void videoInit(int w, int h) {
    static const EGLint attribute_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    VC_DISPMANX_ALPHA_T alpha = {
        DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 255, 0
    };

    EGLint num_config;
    EGLConfig config;

    static EGL_DISPMANX_WINDOW_T nativewindow;

    DISPMANX_ELEMENT_HANDLE_T dispman_element;
    DISPMANX_DISPLAY_HANDLE_T dispman_display;
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    /*This is sort of useless, only needed for the kbd */
	SDL_Init(SDL_INIT_VIDEO);
    SDL_SetVideoMode(0,0,16,SDL_HWSURFACE);

    bcm_host_init();

    display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display,NULL,NULL);
    eglChooseConfig(display,attribute_list,&config,1,&num_config);
    context=eglCreateContext(display,config,EGL_NO_CONTEXT,NULL);

    graphics_get_display_size(0,&width,&height);

    dst_rect.x=0;
    dst_rect.y=0;
    dst_rect.width=width;
    dst_rect.height=height;

    src_rect.x=0;
    src_rect.y=0;
    src_rect.width=((uint32_t)w)<<16;
    src_rect.height=((uint32_t)h)<<16;

    dispman_display=vc_dispmanx_display_open(0);
    dispman_update=vc_dispmanx_update_start(0);

    dispman_element=vc_dispmanx_element_add(dispman_update,
                    dispman_display, 0/*layer*/, &dst_rect, 0/*src*/,
                    &src_rect, DISPMANX_PROTECTION_NONE, &alpha /*alpha*/,
                    0/*clamp*/, 0/*transform*/);

    nativewindow.element=dispman_element;
    nativewindow.width=width;
    nativewindow.height=height;
    vc_dispmanx_update_submit_sync(dispman_update);

    surface = eglCreateWindowSurface(display,config,&nativewindow,NULL);

    eglMakeCurrent(display,surface,surface,context);
}

#endif
