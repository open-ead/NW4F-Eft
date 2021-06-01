#ifndef MATH_VEC4_H_
#define MATH_VEC4_H_

#include <math/math_VEC2.h>

namespace nw { namespace math {

struct VEC4
{
    f32 MagnitudeSquare() const
    {
        return x*x + y*y + z*z + w*w;
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
        w *= inv_mag;
    }

    VEC2& xy()
    {
        return ((VEC2*)this)[0];
    }

    VEC2& zw()
    {
        return ((VEC2*)this)[1];
    }

    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

} } // namespace nw::math

#endif // MATH_VEC4_H_
