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
        GLint infoLen = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLen);
        
        if (infoLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(handle, infoLen, NULL, infoLog);
            printf("%s", infoLog);
            free(infoLog);
        }

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
"#define PI  3.14159\n"
"#define EPS 0.00001\n"
"#define E   2.71828\n"
"#define beat mod(u_time, 1.)\n"
"float rand(vec2 n) {\n"
"    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);\n"
"}\n"
"vec2 rotate(vec2 v, float a) {\n"
"	float s = sin(a);\n"
"	float c = cos(a);\n"
"	mat2 m = mat2(c, -s, s, c);\n"
"	return m * v;\n"
"}\n"
"float stripes(vec2 uv, float f) {\n"
"    return clamp(sin(uv.x*2.*PI*f)*(1./EPS), 0., 1.);\n"
"}\n"
"float freqtransform(float l, float base, float s) {\n"
"    return base + floor(l*s) * base;\n"
"}\n"
"float saturate(float v) {\n"
"    return clamp(v, 0., 1.);\n"
"}\n"
"float circle(vec2 uv, vec2 pos, float r) {\n"
"    return saturate((r-length(uv - pos))*(1./EPS));\n"
"}\n"
"float scene5(vec2 uv, float start) {\n"
"    float t = u_time-start;\n"
"    return 1.-t/4.;\n"
"}\n"
"float scene4(vec2 uv, float start) {\n"
"    float t = u_time-start;\n"
"    t*=0.2;\n"
"    return sin(\n"
"        sin(uv.x*8.+t*0.2)\n"
"        	+sin(uv.y*8.+t*1.3)\n"
"        	+sin(sin(uv.x*12.+uv.y*12.))\n"
"    )*0.5+0.5+beat*sin(t);\n"
"}\n"
"float scene3(vec2 uv, float start) {\n"
"    float t = u_time-start;\n"
"    	float r1 = 2.+beat+t*2.4;\n"
"    	float r2 = 3.+beat+t*1.8;\n"
"    	float r3 = 4.+beat+t*1.1;\n"
"    	vec2 pos = vec2(0., -t*0.2+beat*0.06+0.6);\n"
"       return circle(uv, pos, 0.5)*0.2\n"
"	     + circle(uv, vec2(sin(r1)*0.3+pos.x, cos(r1)*0.3+pos.y), 0.07)*0.2\n"
"        + circle(uv, vec2(sin(r2)*0.5+pos.x, cos(r2)*0.5+pos.y), 0.07)*0.2\n"
"        + circle(uv, vec2(sin(r3)*0.6+pos.x, cos(r3)*0.6+pos.y), 0.1)*0.2;\n"
"}\n"
"float scene2(vec2 uv, float start) {\n"
"    float t = u_time-start;\n"
"    return circle(uv, vec2(0., t*0.2-beat*0.1-0.3), 0.5)*0.2\n"
"        + circle(uv, vec2(beat*t*0.1+t*0.1, -beat*0.5), 0.07)*0.2\n"
"        + circle(uv, vec2(-beat*t*0.2-t*0.08, -beat*0.1), 0.07)*0.2\n"
"        + circle(uv, vec2(-0.7+beat+t*0.1-t*0.12, 0.3-beat*0.2-t*0.07), 0.1)*0.2;\n"
"}\n"
"float scene1(vec2 uv, float start) {\n"
"    float t = u_time-start;\n"
"    t*=0.3;\n"
"    return sin(rotate(uv, t*0.4-beat*0.2).x*2. + beat*0.4 - t*2.)*0.5+0.4;\n"
"}\n"
"float scene(vec2 uv) {                         \n"
"    if (u_time < 12.) {                        \n"
"    	return scene1(uv, 0.);                  \n"
"    } else if (u_time < 12.+6.) {              \n"
"    	return scene2(uv, 12.);                 \n"
"    } else if (u_time < 12.+6.+6.) {           \n"
"        return scene3(uv, 12.+6.);             \n"
"    } else if (u_time < 12.+6.+6.+5.) {         \n"
"        return scene4(uv, 12.+6.+6.);          \n"
"    } else {                                   \n"
"        return scene5(uv, 12.+6.+6.+5.);       \n"
"    }                                          \n"
"}                                              \n"
"void main() {\n"
"	 vec2 uv = (v_texcoord)*2. - vec2(1.);\n"
"    uv.y /= 16./9.;\n"
"    float imageLevels = freqtransform(\n"
"       saturate(scene(uv)),\n"
"       2.,\n"
"       6.\n"
"    );\n"
"    float sep = (1.-beat)*0.004+0.007;\n"
"    sep+=saturate(u_time*0.05-(12.+6.+6.+5.+5.-beat*0.2)*0.05);\n"
"    vec3 c = stripes(\n"
"        rotate(uv, imageLevels/10.),\n"
"        imageLevels\n"
"    )*vec3(0.0, 1.0, 0.0)\n"
"    + stripes(\n"
"        rotate(vec2(uv.x+sep, uv.y), imageLevels/10.),\n"
"        imageLevels\n"
"    )*vec3(1.0, 0.0, 0.0)\n"
"    +stripes(\n"
"        rotate(vec2(uv.x-sep, uv.y), imageLevels/10.),\n"
"        imageLevels\n"
"    )*vec3(0.0, 0.0, 1.0);\n"
"    c-=length(uv)*0.3+beat*0.06;\n"
"	gl_FragColor = vec4(clamp(c,0.,1.)+rand(uv+u_time)*0.06,1.0);\n"
"}\n";
    
    const GLint shader = linkProgram(loadShader(vertShader, GL_VERTEX_SHADER), loadShader(fragShader, GL_FRAGMENT_SHADER));
    const GLint shaderUniformTime = dnload_glGetUniformLocation(shader, "u_time");
    /*const GLint shaderUniformBeat = dnload_glGetUniformLocation(shader, "u_beat");*/

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

        dnload_glUseProgram(shader);
        dnload_glUniform1f(shaderUniformTime, gCurTime);
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
