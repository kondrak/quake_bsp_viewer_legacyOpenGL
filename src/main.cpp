#include "Application.hpp"
#include "InputHandlers.hpp"
#include "renderer/RenderContext.hpp"


// keep the render context and application object global
RenderContext g_renderContext;
Application   g_application;


int main(int argc, char **argv)
{
    // initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) < 0 ) 
    {
        return 1;
    }

    g_renderContext.Init("Quake BSP Viewer", 100, 100, 1024, 768);

    g_application.Initialize(g_renderContext.width, g_renderContext.height);

    SDL_ShowCursor( SDL_DISABLE );

    // initialize Glee
    if (!GLeeInit())
    {
        return 1;
    }

    g_application.OnStart( argc, argv );

    Uint32 last = SDL_GetTicks();

    while( g_application.Running() )
    {
        Uint32 now = SDL_GetTicks();

        processEvents();

        g_application.OnUpdate(float(now - last) / 1000.f);

        g_application.OnRender();

        SDL_GL_SwapWindow( g_renderContext.window );

        last = now;
    }

    g_application.OnTerminate();

    SDL_Quit();

    return 0;
}