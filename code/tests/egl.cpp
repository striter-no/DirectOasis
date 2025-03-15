#include <EGL/egl.h>
#include <GL/gl.h>

const EGLint configAttribs[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_DEPTH_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_NONE
};    

const int pbufferWidth = 900;
const int pbufferHeight = 700;

const EGLint pbattr[] = {
    EGL_WIDTH, pbufferWidth,
    EGL_HEIGHT, pbufferHeight,
    EGL_NONE,
};

int main(int argc, char *argv[]){
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major, minor;
    eglInitialize(eglDpy, &major, &minor);
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbattr);
    eglBindAPI(EGL_OPENGL_API);
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, 0);    
    eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

    ; // OpenGL Stuff

    eglTerminate(eglDpy);
    return 0;
}