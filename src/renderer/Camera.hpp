#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED

#include "renderer/Math.hpp"

/*
 *  DOF6/FPS camera
 */

class Camera
{
public:
    enum CameraMode
    {
        CAM_DOF6,
        CAM_FPS
    };

    Camera(float x, float y, float z);

    Camera(const Math::Vector3f &position,
           const Math::Vector3f &up,
           const Math::Vector3f &right,
           const Math::Vector3f &view );

    void OnRender();
    void RotateCamera(float angle, float x, float y, float z);

    void Move( const Math::Vector3f &Direction );
    void MoveForward( float Distance );
    void  MoveUpward( float Distance );
    void Strafe( float Distance );    

    void SetMode(CameraMode cm) { m_mode = cm; }
    Math::Vector3f Position()   { return m_position; }

    // rotate in Euler-space - used mainly for some debugging
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);

    virtual void OnMouseMove(int x, int y);

private:
    CameraMode     m_mode;
    Math::Vector3f m_position;
    float          m_yLimit;

    // camera orientation vectors
    Math::Vector3f m_viewVector;
    Math::Vector3f m_rightVector;	
    Math::Vector3f m_upVector;

    // current rotation info (angles in radian)
    Math::Vector3f m_rotation;
};

#endif