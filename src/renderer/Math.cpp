#include "renderer/Math.hpp"

namespace Math
{

/*
 * Vector
 */
    void Vector3f::Normalize()
    {
        float l = Length(); 
        if (l == 0.0f) return;
        m_x /= l;
        m_y /= l;
        m_z /= l;
    }

    void Vector3f::QuickNormalize()
    {
        float l = QuickInverseSqrt( m_x*m_x + m_y*m_y + m_z*m_z );
        m_x *= l;
        m_y *= l;
        m_z *= l;
    }

    Vector3f Vector3f::CrossProduct(const Vector3f &v2) const
    {
        return Vector3f( m_y*v2.m_z - m_z*v2.m_y,
                         m_z*v2.m_x - m_x*v2.m_z,
                         m_x*v2.m_y - m_y*v2.m_x );
    }

    float Vector3f::DotProduct(const Vector3f &v2) const
    {
        return m_x*v2.m_x + m_y*v2.m_y + m_z*v2.m_z;
    }

    Vector3f Vector3f::operator+(const Vector3f &v2) const 
    {
        return Vector3f( m_x + v2.m_x,
                         m_y + v2.m_y,
                         m_z + v2.m_z );;
    }

    Vector3f Vector3f::operator-(const Vector3f &v2) const
    {
        return Vector3f( m_x-v2.m_x,
                         m_y-v2.m_y,
                         m_z-v2.m_z );
    }

    Vector3f Vector3f::operator*( float r ) const
    {
        return Vector3f( m_x*r,
                         m_y*r,
                         m_z*r );
    }


/*
 *  Quaternion
 */
    Quaternion Quaternion::GetConjugate() const
    {
        return Quaternion( -m_x, -m_y, -m_z, m_w);
    }

    void Quaternion::Normalize()
    {
        float l = sqrt( m_x*m_x + m_y*m_y + m_z*m_z + m_w*m_w );
        m_x /= l;
        m_y /= l;
        m_z /= l;
        m_w /= l;
    }

    void Quaternion::QuickNormalize()
    {
        float l = QuickInverseSqrt( m_x*m_x + m_y*m_y + m_z*m_z + m_w*m_w );
        m_x *= l;
        m_y *= l;
        m_z *= l;
        m_w *= l;
    }

    Quaternion Quaternion::operator*(const Quaternion &q2) const
    {
        return Quaternion( m_w*q2.m_x + m_x*q2.m_w + m_y*q2.m_z - m_z*q2.m_y,
                           m_w*q2.m_y - m_x*q2.m_z + m_y*q2.m_w + m_z*q2.m_x,
                           m_w*q2.m_z + m_x*q2.m_y - m_y*q2.m_x + m_z*q2.m_w,
                           m_w*q2.m_w - m_x*q2.m_x - m_y*q2.m_y - m_z*q2.m_z );
    }

    Vector3f Quaternion::operator*(const Vector3f &vec) const
    {
        Vector3f vn(vec);
        vn.QuickNormalize();

        Quaternion vecQuat( vn.m_x,
                            vn.m_y,
                            vn.m_z,
                            0.0f );

        Quaternion resQuat( vecQuat * GetConjugate() );
        resQuat = *this * resQuat;

        return Vector3f(resQuat.m_x, resQuat.m_y, resQuat.m_z);
    }


    /*
     * General purpose functions
     */
    float QuickInverseSqrt( float number )
    {
        long i;
        float x2, y;

        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;
        i  = 0x5f3759df - ( i >> 1 );
        y  = * ( float * ) &i;
        y  = y * ( 1.5f - ( x2 * y * y ) );   // 1st iteration
        y  = y * ( 1.5f - ( x2 * y * y ) );   // 2nd iteration

        return y;
    }

    int PointPlanePos(float normalX, float normalY, float normalZ, float intercept, const Math::Vector3f &point)
    {
        float distance = point.m_x * normalX + point.m_y * normalY + point.m_z * normalZ - intercept;

        if(distance >= 0.0f)
            return PointInFrontOfPlane;

        return PointBehindPlane; 
    }
}