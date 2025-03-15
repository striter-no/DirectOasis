#pragma once
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GL/gl.h>

class GLRenderer {
        const EGLint *getConfig(
            int blue_size,
            int green_size,
            int red_size,
            int depth_size
        ){
            const EGLint attribs[] = {
                EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
                EGL_BLUE_SIZE, blue_size,
                EGL_GREEN_SIZE, green_size,
                EGL_RED_SIZE, red_size,
                EGL_DEPTH_SIZE, depth_size,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
                EGL_NONE
            };
            return attribs;
        }

        const EGLint *getWindowConfig(
            int width,
            int height
        ){
            const EGLint pbattr[] = {
                EGL_WIDTH, width,
                EGL_HEIGHT, height,
                EGL_NONE,
            };

            return pbattr;
        }

        int blue_size = 8, green_size = 8, red_size = 8, depth_size = 8;
        int width = 900, height = 700;
        EGLDisplay eglDpy;

    public:

        void setup(){
            auto gen_conf = getConfig(blue_size, green_size, red_size, depth_size);
            auto win_conf = getWindowConfig(width, height);

            eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            EGLint major, minor;
            eglInitialize(eglDpy, &major, &minor);
            EGLint numConfigs;
            EGLConfig eglCfg;

            eglChooseConfig(eglDpy, gen_conf, &eglCfg, 1, &numConfigs);
            EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, win_conf);
            eglBindAPI(EGL_OPENGL_API);
            EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, 0);    
            eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
        }

        void finish(){
            eglTerminate(eglDpy);
        }

        GLRenderer(
            int width,
            int height,
            int blue_size = 8,
            int green_size = 8,
            int red_size = 8,
            int depth_size = 8
        ): width(width), 
           height(height), 
           blue_size(blue_size), 
           green_size(green_size), 
           red_size(red_size), 
           depth_size(depth_size) 
        {}

        GLRenderer(){}
        ~GLRenderer(){}
};