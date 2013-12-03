#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <map>
#include "InputHandlers.hpp"


/*
 * main application 
 */

class Application
{
public:
    Application();
    void Initialize(int width, int height);

    void OnStart(int argc, char **argv);
    void OnRender();
    void OnUpdate(float dt); 

    inline bool Running() const  { return m_running; }
    inline void Terminate()      { m_running = false; }

    void OnTerminate();
    bool KeyPressed(KeyCode key);
    void OnKeyPress(KeyCode key);
    void OnKeyRelease(KeyCode key);
    void OnMouseMove(int x, int y);
    void LogError(const char *msg);

private:
    void SetupPerspective();  // setup perspective view
    void SetupOrthogonal();   // setup orthogonal view
    void UpdateCamera( float dt );

    inline void SetKeyPressed(KeyCode key, bool pressed) { m_keyStates[ key ] = pressed; }

    bool m_running;
    std::map< KeyCode, bool > m_keyStates; 
};

#endif
