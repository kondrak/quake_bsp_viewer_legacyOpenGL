#ifndef FRUSTUM_INCLUDED
#define FRUSTUM_INCLUDED

#include "renderer/OpenGLHeaders.hpp"
#include "renderer/Math.hpp"


/*
 * View frustum 
 */

struct Plane
{
    float A, B, C, D;
};

class Frustum
{
public:
    void OnRender();
    bool BoxInFrustum(const Math::Vector3f *vertices);

private:
    void ExtractPlane(Plane &plane, GLfloat *mat, int row);
    Plane m_planes[6];
};

#endif