#ifndef MATH_INCLUDED
#define MATH_INCLUDED

#include <math.h>

/*
 * Basic math structures (vectors, quaternions)
 */

#define PI 3.1415926535897932384626433832795
#define PIdiv180 0.01745329251


namespace Math
{
    // 2D vector
    struct Vector2f
    {
        Vector2f() : m_x(0.0f), m_y(0.0f)
        {
        }

        Vector2f( float x, float y) : m_x(x), m_y(y)
        {
        }

        float m_x;
        float m_y;
    };


    // 3D vector
    class Vector3f
    {
    public:
        Vector3f() : m_x(0.0f), m_y(0.0f), m_z(0.0f)
        {
        }
        Vector3f(float x, float y, float z) : m_x(x), m_y(y), m_z(z)
        {
        }

        Vector3f(const Vector3f &v2) : m_x(v2.m_x), m_y(v2.m_y), m_z(v2.m_z)
        {
        }

        float Length()
        {
            return sqrt( m_x*m_x + m_y*m_y + m_z*m_z );
        }

        void Normalize();
        void QuickNormalize(); // normalize with Q_rsqrt
        Vector3f CrossProduct(const Vector3f &v2) const;
        float DotProduct(const Vector3f &v2) const;

        float m_x;
        float m_y;
        float m_z;

        Vector3f operator+ (const Vector3f &v2) const;
        Vector3f operator- (const Vector3f &v2) const;
        Vector3f operator* ( float r) const;           // scale the vector by r
    };


    // quaternion
    class Quaternion
    {
    public:       
        Quaternion() : m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(0.0f)
        {
        }

        // create based on standard parameters
        Quaternion(float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w)
        {
        }

        // create from axis/angle representaiton
        Quaternion(const Vector3f &axis, float angle) : m_x( axis.m_x * sin(angle/2) ),
                                                        m_y( axis.m_y * sin(angle/2) ),
                                                        m_z( axis.m_z * sin(angle/2) ),
                                                        m_w( cos(angle/2) )
        {
        }

        Quaternion(const Quaternion &q2) : m_x(q2.m_x), m_y(q2.m_y), m_z(q2.m_z), m_w(q2.m_w)
        {
        }

        Quaternion GetConjugate() const;
        void Normalize();
        void QuickNormalize(); // normalize with Q_rsqrt

        float m_x;
        float m_y;
        float m_z;
        float m_w;

        Vector3f   operator*(const Vector3f &vec)  const;   // apply quat-rotation to vec
        Quaternion operator*(const Quaternion &q2) const;
    };


    // general purpose functions
    enum PointPlanePosition
    {
        PointBehindPlane,
        PointInFrontOfPlane
    };

    // quick inverse square root
    float QuickInverseSqrt( float number );

    // determine whether a point is in front of or behind a plane (based on its normal vector)
    int PointPlanePos(float normalX, float normalY, float normalZ, float intercept, const Math::Vector3f &point);
}
#endif