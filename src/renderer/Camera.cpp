#include "renderer/Camera.hpp"
#include "renderer/RenderContext.hpp"
#include "renderer/OpenGLHeaders.hpp"

extern RenderContext g_renderContext;

static const float MouseSensitivity = 800.f;


Camera::Camera(float x, float y, float z) : m_position(x, y, z),
                                            m_mode(CAM_DOF6),
                                            m_yLimit(1.f),
                                            m_viewVector(0.0f, 0.0f, -1.0f),
                                            m_rightVector(1.0f, 0.0f, 0.0f),
                                            m_upVector(0.0f, 1.0f, 0.0f),
                                            m_rotation(0.0f, 0.0f, 0.0f)
{
}


Camera::Camera(const Math::Vector3f &position,
               const Math::Vector3f &up,
               const Math::Vector3f &right,
               const Math::Vector3f &view ) : m_position(position),
                                              m_mode(CAM_DOF6),
                                              m_yLimit(1.f),
                                              m_viewVector(view),
                                              m_rightVector(right),
                                              m_upVector(up),
                                              m_rotation(0.0f, 0.0f, 0.0f)
{
}


void Camera::OnRender()
{
    // point we're looking at
    Math::Vector3f ViewPoint = m_position+m_viewVector;

    gluLookAt(  m_position.m_x, m_position.m_y, m_position.m_z,
                ViewPoint.m_x, ViewPoint.m_y, ViewPoint.m_z,
                m_upVector.m_x, m_upVector.m_y, m_upVector.m_z);

}


void Camera::RotateCamera(float angle, float x, float y, float z)
{
    Math::Quaternion result;

    // create quaternion from axis-angle
    Math::Quaternion rotQuat( Math::Vector3f(x, y, z), angle );

    Math::Quaternion viewQuat( m_viewVector.m_x,
                               m_viewVector.m_y,
                               m_viewVector.m_z,
                               0 );

    result = ( ( rotQuat * viewQuat) * rotQuat.GetConjugate() );

    m_viewVector.m_x = result.m_x;
    m_viewVector.m_y = result.m_y;
    m_viewVector.m_z = result.m_z;
}


void Camera::Move(const Math::Vector3f &Direction)
{
    m_position = m_position + Direction;
}


void Camera::MoveForward( float Distance )
{
    m_position = m_position + ( m_viewVector * -Distance );
}


void Camera::MoveUpward( float Distance )
{
    m_position = m_position + ( m_upVector * Distance );
}


void Camera::Strafe( float Distance )
{
    m_position = m_position + ( m_rightVector * Distance );
}


void Camera::rotateX(float angle)
{
    m_rotation.m_x += angle;

    RotateCamera( angle, m_rightVector.m_x, m_rightVector.m_y, m_rightVector.m_z);
    m_upVector = m_rightVector.CrossProduct( m_viewVector );
}


void Camera::rotateY(float angle)
{
    m_rotation.m_y += angle;
    RotateCamera( angle, m_upVector.m_x, m_upVector.m_y, m_upVector.m_z);
    m_rightVector = m_viewVector.CrossProduct( m_upVector );
}


void Camera::rotateZ(float angle)
{
    m_rotation.m_z += angle;

    Math::Vector3f axis = m_viewVector;
    axis.QuickNormalize();

    Math::Quaternion rotQuat( axis, angle );

    Math::Quaternion viewQuat( m_upVector.m_x,
        m_upVector.m_y,
        m_upVector.m_z,
        0 );

    Math::Quaternion result = ( ( rotQuat * viewQuat) * rotQuat.GetConjugate() );

    m_upVector.m_x = result.m_x;
    m_upVector.m_y = result.m_y;
    m_upVector.m_z = result.m_z;

    m_rightVector = m_upVector.CrossProduct( m_viewVector ) * -1;
}


void Camera::OnMouseMove(int x, int y)
{ 
    // vector that describes mouseposition - center
    Math::Vector3f MouseDirection(0, 0, 0);

    // skip if cursor didn't move
    if((x == g_renderContext.halfWidth) && (y == g_renderContext.halfHeight))
        return;

    // keep the cursor at screen center
    SDL_WarpMouseInWindow(g_renderContext.window, g_renderContext.halfWidth, g_renderContext.halfHeight);

    MouseDirection.m_x = (float)(g_renderContext.halfWidth - x)/MouseSensitivity; 
    MouseDirection.m_y = (float)(g_renderContext.halfHeight - y)/MouseSensitivity;

    m_rotation.m_x += MouseDirection.m_y;
    m_rotation.m_y += MouseDirection.m_x;

    // camera up-down movement limit
    if(m_mode == CAM_FPS)
    {
        if(m_rotation.m_x  > m_yLimit)
        {
            MouseDirection.m_y = m_yLimit + MouseDirection.m_y - m_rotation.m_x;
            m_rotation.m_x  = m_yLimit;
        }

        if(m_rotation.m_x < -m_yLimit)
        {
            MouseDirection.m_y = -m_yLimit + MouseDirection.m_y - m_rotation.m_x;
            m_rotation.m_x  = -m_yLimit;
        }
    }

    // get the axis to rotate around the x-axis. 
    Math::Vector3f Axis = m_viewVector.CrossProduct( m_upVector );
    // normalize to properly use the conjugate
    Axis.QuickNormalize();

    RotateCamera( MouseDirection.m_y, Axis.m_x, Axis.m_y, Axis.m_z );

    if(m_mode == CAM_DOF6)
        m_upVector = m_rightVector.CrossProduct( m_viewVector );

    if(m_mode == CAM_FPS)
        RotateCamera(MouseDirection.m_x, m_upVector.m_x, m_upVector.m_y, m_upVector.m_z); 
    else
    {
        // Rotate in horizontal plane
        Math::Vector3f Axis2( m_viewVector.CrossProduct( m_rightVector ) );
        Axis2.QuickNormalize();

        RotateCamera(-MouseDirection.m_x, Axis2.m_x, Axis2.m_y, Axis2.m_z);
    }

    m_rightVector = m_viewVector.CrossProduct( m_upVector );    
    m_rightVector.QuickNormalize();
}
