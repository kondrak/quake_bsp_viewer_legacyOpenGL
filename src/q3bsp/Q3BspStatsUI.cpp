#include <sstream>
#include "q3bsp/Q3BspMap.hpp"
#include "q3bsp/Q3BspStatsUI.hpp"


Q3StatsUI::Q3StatsUI(BspMap *map) : StatsUI(map)
{
    glGenTextures(1, &m_fontTex);
    m_font.Create("res/font.glf", m_fontTex);
}


void Q3StatsUI::OnRenderStart()
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
}


void Q3StatsUI::Render()
{
    m_font.Begin();
    m_font.SetScale(0.01f);

    std::stringstream statsStream;  

    if(!m_map)
    {
        m_font.SetColor(1.f, 0.f, 0.f);
        statsStream << "Error loading BSP - missing/corrupted file or no file specified!";
        m_font.PrintText( statsStream.str(), 0.20f, 0.45f, 0.f);
        return;
    }

    const BspStats &stats = m_map->GetMapStats();

    statsStream << "Total vertices: " << stats.totalVertices;
    m_font.PrintText( statsStream.str(), 0.01f, 0.12f, 0.f);

    statsStream.str("");
    statsStream << "Total faces: " << stats.totalFaces;
    m_font.PrintText( statsStream.str(), 0.01f, 0.15f, 0.f);

    statsStream.str("");
    statsStream << "Total patches: " << stats.totalPatches;
    m_font.PrintText( statsStream.str(), 0.01f, 0.18f, 0.f);

    statsStream.str("");
    statsStream << "Rendered faces: " << stats.visibleFaces;
    m_font.PrintText( statsStream.str(), 0.01f, 0.21f, 0.f);

    statsStream.str("");
    statsStream << "Rendered patches: " << stats.visiblePatches;
    m_font.PrintText( statsStream.str(), 0.01f, 0.24f, 0.);

    m_font.PrintText(" ~  - toggle stats view", 0.70f, 0.72f, 0.f);
    if( m_map->HasRenderFlag(Q3RenderShowWireframe) )
        m_font.SetColor(0.f, 1.f, 0.f);
    m_font.PrintText("F1 - show wireframe", 0.70f, 0.75f, 0.f);
    m_font.SetColor(1.f, 1.f, 1.f);

    if( m_map->HasRenderFlag(Q3RenderShowLightmaps) )
        m_font.SetColor(0.f, 1.f, 0.f);
    m_font.PrintText("F2 - show lightmaps", 0.70f, 0.78f, 0.f);
    m_font.SetColor(1.f, 1.f, 1.f);

    if( m_map->HasRenderFlag(Q3RenderUseLightmaps) )
        m_font.SetColor(0.f, 1.f, 0.f);
    m_font.PrintText("F3 - use lightmaps", 0.70f, 0.81f, 0.f);
    m_font.SetColor(1.f, 1.f, 1.f);

    if( m_map->HasRenderFlag(Q3RenderAlphaTest) )
        m_font.SetColor(0.f, 1.f, 0.f);
    m_font.PrintText("F4 - use alpha test", 0.70f, 0.84f, 0.f);
    m_font.SetColor(1.f, 1.f, 1.f);

    if( !m_map->HasRenderFlag(Q3RenderSkipMissingTex) )
        m_font.SetColor(0.f, 1.f, 0.f);
    m_font.PrintText("F5 - show missing textures", 0.70f, 0.87f, 0.f);
    m_font.SetColor(1.f, 1.f, 1.f);

    if( !m_map->HasRenderFlag(Q3RenderSkipPVS) )
        m_font.SetColor(0.f, 1.f, 0.f);
    m_font.PrintText("F6 - use PVS culling", 0.70f, 0.90f, 0.f);
    m_font.SetColor(1.f, 1.f, 1.f);

    if( !m_map->HasRenderFlag(Q3RenderSkipFC) )
        m_font.SetColor(0.f, 1.f, 0.f);
    m_font.PrintText("F7 - use frustum culling", 0.70f, 0.93f, 0.f);
    m_font.SetColor(1.f, 1.f, 1.f);

    //reset the color
    glColor3f(1.f, 1.f, 1.f);
}


void Q3StatsUI::OnRenderFinish()
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);
}