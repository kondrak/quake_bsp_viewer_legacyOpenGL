#ifndef Q3BSPSTATSUI_INCLUDED
#define Q3BSPSTATSUI_INCLUDED

#include "common/StatsUI.hpp"
#include "renderer/OpenGLHeaders.hpp"
#include "GLFont/glfont.h"

/*
 *  Quake III BSP stats display
 */

class Q3StatsUI : public StatsUI
{
public:
    Q3StatsUI(BspMap *map);

    void OnRenderStart();
    void Render();
    void OnRenderFinish();

private:
    GLuint m_fontTex;
    GLFont m_font;
};

#endif