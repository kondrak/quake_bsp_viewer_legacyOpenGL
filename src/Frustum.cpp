#include "Frustum.hpp"


void Frustum::OnRender()
{
    // do the math on OpenGL projection and modelview matrices
    GLfloat projection[16];
    GLfloat  modelview[16];

    glGetFloatv(GL_PROJECTION_MATRIX, projection);
    glGetFloatv(GL_MODELVIEW_MATRIX,  modelview);

    glPushMatrix();
    glLoadMatrixf(projection);
    glMultMatrixf(modelview);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    glPopMatrix();

    // extract each plane
    ExtractPlane(m_planes[0], modelview, 1);
    ExtractPlane(m_planes[1], modelview, -1);
    ExtractPlane(m_planes[2], modelview, 2);
    ExtractPlane(m_planes[3], modelview, -2);
    ExtractPlane(m_planes[4], modelview, 3);
    ExtractPlane(m_planes[5], modelview, -3);
}

bool Frustum::BoxInFrustum(const Math::Vector3f *vertices)
{
    for(int i = 0; i < 6; ++i)
    {
        if((m_planes[i].A * vertices[0].m_x + m_planes[i].B * vertices[0].m_y + m_planes[i].C * vertices[0].m_z  + m_planes[i].D) > 0)
            continue;
        if((m_planes[i].A * vertices[1].m_x + m_planes[i].B * vertices[1].m_y + m_planes[i].C * vertices[1].m_z  + m_planes[i].D) > 0)
            continue;
        if((m_planes[i].A * vertices[2].m_x + m_planes[i].B * vertices[2].m_y + m_planes[i].C * vertices[2].m_z  + m_planes[i].D) > 0)
            continue;
        if((m_planes[i].A * vertices[3].m_x + m_planes[i].B * vertices[3].m_y + m_planes[i].C * vertices[3].m_z  + m_planes[i].D) > 0)
            continue;
        if((m_planes[i].A * vertices[4].m_x + m_planes[i].B * vertices[4].m_y + m_planes[i].C * vertices[4].m_z  + m_planes[i].D) > 0)
            continue;
        if((m_planes[i].A * vertices[5].m_x + m_planes[i].B * vertices[5].m_y + m_planes[i].C * vertices[5].m_z  + m_planes[i].D) > 0)
            continue;
        if((m_planes[i].A * vertices[6].m_x + m_planes[i].B * vertices[6].m_y + m_planes[i].C * vertices[6].m_z  + m_planes[i].D) > 0)
            continue;
        if((m_planes[i].A * vertices[7].m_x + m_planes[i].B * vertices[7].m_y + m_planes[i].C * vertices[7].m_z  + m_planes[i].D) > 0)
            continue;

        return false;
    }

    return true;
}


// extract a plane from a given matrix and row id
void Frustum::ExtractPlane(Plane &plane, GLfloat *mat, int row)
{
    int scale = (row < 0) ? -1 : 1;
    row = abs(row) - 1;

    // calculate plane coefficients from the matrix
    plane.A =  mat[3]  + scale * mat[row + 0];
    plane.B =  mat[7]  + scale * mat[row + 4];
    plane.C =  mat[11] + scale * mat[row + 8];
    plane.D =  mat[15] + scale * mat[row + 12];

    // normalize the plane
    float length = Math::QuickInverseSqrt( plane.A * plane.A + plane.B * plane.B + plane.C * plane.C );

    plane.A *= length;
    plane.B *= length;
    plane.C *= length;
    plane.D *= length;
}