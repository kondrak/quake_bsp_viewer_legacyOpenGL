#include "renderer/RenderContext.hpp"


void RenderContext::Init(const char *title, int x, int y, int w, int h)
{
    window = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GL_CreateContext(window); 

    SDL_GetWindowSize(window, &width, &height);

    halfWidth  = width  >> 1;
    halfHeight = height >> 1;
}