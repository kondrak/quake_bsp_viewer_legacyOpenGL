#include "InputHandlers.hpp"
#include "Application.hpp"

extern Application g_application;


KeyCode SDLKeyToKeyCode( SDL_Keycode key )
{
    switch(key)
    {
    case SDLK_1:
        return KEY_1;
    case SDLK_2:
        return KEY_2;
    case SDLK_a:
        return KEY_A;
    case SDLK_d:
        return KEY_D;
    case SDLK_e:
        return KEY_E;
    case SDLK_f:
        return KEY_F;
    case SDLK_i:
        return KEY_I;
    case SDLK_j:
        return KEY_J;
    case SDLK_k:
        return KEY_K;
    case SDLK_l:
        return KEY_L;
    case SDLK_s:
        return KEY_S;
    case SDLK_o:
        return KEY_O;
    case SDLK_q:
        return KEY_Q;
    case SDLK_r:
        return KEY_R;
    case SDLK_u:
        return KEY_U;
    case SDLK_w:
        return KEY_W;
    case SDLK_ESCAPE:
        return KEY_ESC;
    case SDLK_F1:
        return KEY_F1;
    case SDLK_F2:
        return KEY_F2;
    case SDLK_F3:
        return KEY_F3;
    case SDLK_F4:
        return KEY_F4;
    case SDLK_F5:
        return KEY_F5;
    case SDLK_F6:
        return KEY_F6;
    case SDLK_F7:
        return KEY_F7;
    case SDLK_F8:
        return KEY_F8;
    case SDLK_F9:
        return KEY_F9;
    case SDLK_F10:
        return KEY_F10;
    case SDLK_F11:
        return KEY_F11;
    case SDLK_F12:
        return KEY_F12;
    case SDLK_BACKQUOTE:
        return KEY_TILDE;
    }

    return KEY_NULL;
}


void processEvents()
{
    SDL_Event event;

    while( SDL_PollEvent( &event ) )
    {
        switch(event.type)
        {
        case SDL_KEYDOWN:
            // alt + f4 handling
            if(event.key.keysym.sym == SDLK_F4 && (event.key.keysym.mod == KMOD_LALT || event.key.keysym.mod == KMOD_RALT) ) 
            {
                g_application.Terminate(); 
                break;
            }
            g_application.OnKeyPress( SDLKeyToKeyCode( event.key.keysym.sym ) );
            break;
        case SDL_KEYUP:
            g_application.OnKeyRelease( SDLKeyToKeyCode( event.key.keysym.sym ) );
            break;
        case SDL_MOUSEMOTION:
            g_application.OnMouseMove(event.motion.x, event.motion.y);
            break;
        case SDL_QUIT:
            g_application.Terminate();
            break;
        // window events      
        case SDL_WINDOWEVENT:
            switch(event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                g_application.Initialize(event.window.data1, event.window.data2);
                break;
            }
            break;
        }
    }
}
