#include <SDL.h>
#include "Application.hpp"
#include "renderer/CameraDirector.hpp"
#include "renderer/RenderContext.hpp"
#include "q3bsp/Q3BspLoader.hpp"
#include "Frustum.hpp"
#include "q3bsp/Q3BspStatsUI.hpp"


extern RenderContext g_renderContext;
CameraDirector g_cameraDirector;

BspMap  *g_q3map   = NULL;  // loaded map
StatsUI *g_q3stats = NULL;  // map stats UI
Frustum  g_frustum;         // view frustum


Application::Application() : m_running(true)
{
    g_cameraDirector.AddCamera( Math::Vector3f(0.0f, 0.0f, 4.0f),
                                Math::Vector3f(0.f, 0.f, 1.f),
                                Math::Vector3f(1.f, 0.f, 0.f),
                                Math::Vector3f(0.f, 1.f, 0.f) );

    g_cameraDirector.GetActiveCamera()->SetMode(Camera::CAM_FPS);
}


void Application::Initialize(int width, int height)
{
    g_renderContext.width = width;
    g_renderContext.height = height;

    glEnable( GL_DEPTH_TEST );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(g_renderContext.fov, (float)width / (float)height, g_renderContext.nearPlane, g_renderContext.farPlane);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void Application::OnStart(int argc, char **argv)
{
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    Q3BspLoader loader;

    // assume the first parameter is the map filename
    if(argc > 1)
        g_q3map = loader.Load(argv[1]);

    if(g_q3map)
    {
        g_q3map->Init();
        g_q3map->ToggleRenderFlag(Q3RenderDisplayStats);
        g_q3map->ToggleRenderFlag(Q3RenderUseLightmaps);
    }

    g_q3stats = new Q3StatsUI(g_q3map);
}


void Application::OnRender()
{
    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // render the bsp
    if(g_q3map)
    {
        SetupPerspective();

        g_cameraDirector.GetActiveCamera()->OnRender();        
        g_frustum.OnRender();

        g_q3map->OnRenderStart();
        g_q3map->Render();
        g_q3map->OnRenderFinish();

        if( !g_q3map->HasRenderFlag(Q3RenderDisplayStats) )
            return;
    }

    // render map stats  
    if(g_q3stats)
    {
        SetupOrthogonal();

        g_q3stats->OnRenderStart();
        g_q3stats->Render();
        g_q3stats->OnRenderFinish();
    }
}


void Application::OnUpdate(float dt)
{    
    UpdateCamera( dt );
}


void Application::OnTerminate()
{
    delete g_q3map;
    delete g_q3stats;
}


bool Application::KeyPressed(KeyCode key)
{
    // to be 100% no undefined state exists
    if( m_keyStates.find( key ) == m_keyStates.end() )
        m_keyStates[ key ] = false;

    return m_keyStates[ key ];
}


void Application::OnKeyPress(KeyCode key)
{
    SetKeyPressed( key, true );

    switch(key)
    {
    case KEY_F1:
        g_q3map->ToggleRenderFlag(Q3RenderShowWireframe);
        break;
    case KEY_F2:
        g_q3map->ToggleRenderFlag(Q3RenderShowLightmaps);
        break;
    case KEY_F3:
        g_q3map->ToggleRenderFlag(Q3RenderUseLightmaps);
        break;
    case KEY_F4:
        g_q3map->ToggleRenderFlag(Q3RenderAlphaTest);
        break;
    case KEY_F5:
        g_q3map->ToggleRenderFlag(Q3RenderSkipMissingTex);
        break;
    case KEY_F6:
        g_q3map->ToggleRenderFlag(Q3RenderSkipPVS);
        break;
    case KEY_F7:
        g_q3map->ToggleRenderFlag(Q3RenderSkipFC);
        break;
    case KEY_TILDE:
        g_q3map->ToggleRenderFlag(Q3RenderDisplayStats);
        break;
    case KEY_ESC:
        Terminate();
        break;
    default:
        break;
    }
}


void Application::OnKeyRelease(KeyCode key)
{
    SetKeyPressed( key, false );
}


void Application::OnMouseMove(int x, int y)
{
    g_cameraDirector.GetActiveCamera()->OnMouseMove( x, y );
}


void Application::LogError(const char *msg)
{
    // basic error logging for VS debugger
#ifdef _WIN32
    OutputDebugStringA( msg );
#else
    printf("%s\n", msg);
#endif
}

void Application::SetupPerspective()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(g_renderContext.fov, (float)g_renderContext.width / (float)g_renderContext.height, g_renderContext.nearPlane, g_renderContext.farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void Application::SetupOrthogonal()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0.0, 1.0f, 1.0f, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void Application::UpdateCamera( float dt )
{
    if( KeyPressed(KEY_A) )
        g_cameraDirector.GetActiveCamera()->Strafe(-8.f * dt);

    if( KeyPressed(KEY_D) )
        g_cameraDirector.GetActiveCamera()->Strafe(8.f * dt); 

    if( KeyPressed(KEY_W) )
        g_cameraDirector.GetActiveCamera()->MoveForward(-8.f * dt);

    if( KeyPressed(KEY_S) )
        g_cameraDirector.GetActiveCamera()->MoveForward(8.f * dt);

    // do the barrel roll!
    if( KeyPressed(KEY_Q) )
        g_cameraDirector.GetActiveCamera()->rotateZ( 2.f * dt );

    if( KeyPressed(KEY_E) )
        g_cameraDirector.GetActiveCamera()->rotateZ( -2.f * dt );

    // move straight up/down
    if( KeyPressed(KEY_R) )
        g_cameraDirector.GetActiveCamera()->MoveUpward( 8.f * dt );

    if( KeyPressed(KEY_F) )
        g_cameraDirector.GetActiveCamera()->MoveUpward( -8.f * dt );
}
