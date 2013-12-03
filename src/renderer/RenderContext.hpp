#ifndef RENDERCONTEXT_INCLUDED
#define RENDERCONTEXT_INCLUDED

#include "renderer/OpenGLHeaders.hpp"
#include <SDL.h>

/*
 * SDL-based OpenGL render context
 */

class RenderContext
{
public:
    RenderContext() : window(NULL), 
                      fov(75), 
                      nearPlane(0.1f), 
                      farPlane(1000.f), 
                      width(0), 
                      height(0), 
                      halfWidth(0), 
                      halfHeight(0)
    {
    }

    void Init(const char *title, int x, int y, int w, int h);

    SDL_Window *window;
    int   fov;
    float nearPlane;
    float farPlane;
    int   width;
    int   height;
    int   halfWidth;
    int   halfHeight;
};

#endif