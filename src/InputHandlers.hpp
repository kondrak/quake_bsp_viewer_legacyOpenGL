#ifndef INPUTHANDLERS_INCLUDED
#define INPUTHANDLERS_INCLUDED

#include <SDL.h>

/*
 * application input/event handlers
 */

enum KeyCode
{
    KEY_NULL,
    KEY_1,
    KEY_2,
    KEY_A,      
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_O,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_U,
    KEY_W,
    KEY_ESC,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_TILDE
};

// handler functions
KeyCode SDLKeyToKeyCode( SDL_Keycode key );
void processEvents();

#endif