#ifndef MATH_VEC3_H_
#define MATH_VEC3_H_

#include <cmath>
#include <types.h>

#include <cafe.h>

namespace nw { namespace math {

struct MTX34;

struct VEC3
{
    static const VEC3& Zero()
    {
        static const VEC3 zero = (VEC3){ 0.0f, 0.0f, 0.0f };
        return zero;
    }

    VEC3& operator+=(const VEC3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    VEC3& operator-=(const VEC3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    VEC3& operator*=(f32 a)
    {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }

    VEC3 operator+(const VEC3& rhs) const
    {
        VEC3 ret = *this;
        return (ret += rhs);
    }

    VEC3 operator-(const VEC3& rhs) const
    {
        VEC3 ret = *this;
        return (ret -= rhs);
    }

    VEC3 operator*(f32 a) const
    {
        VEC3 ret = *this;
        return (ret *= a);
    }

    f32 MagnitudeSquare() const
    {
        return x*x + y*y + z*z;
    }

    f32 Magnitude() const
    {
        return sqrtf(MagnitudeSquare());
    }

    void Normalize()
    {
        f32 inv_mag = 1.0f / Magnitude(); // No division by zero check...
        x *= inv_mag;
        y *= inv_mag;
        z *= inv_mag;
    }

    static inline VEC3* MultMTX(VEC3* dst, const VEC3* a, const MTX34* b);

    static VEC3* Scale(VEC3* dst, const VEC3* src, f32 a)
    {
        // Why does this even exist
        ASM_VECScale((const Vec*)src, (Vec*)dst, a);
        return dst;
    }

    f32 x;
    f32 y;
    f32 z;
};

} } // namespace nw::math

#endif // MATH_VEC3_H_
