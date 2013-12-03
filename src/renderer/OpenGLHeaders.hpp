#ifndef OPENGLHEADERS_INCLUDED
#define OPENGLHEADERS_INCLUDED

#ifdef _WIN32
    #include <Windows.h>
    #include "GLee/GLee.h"
    #include <gl/GL.h>
    #include <gl/GLU.h>
#else
    #error "Only Windows platform supported for now."
#endif

#endif