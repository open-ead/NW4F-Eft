#ifndef MATH_VEC3_H_
#define MATH_VEC3_H_

#include <cmath>
#include <types.h>

namespace nw { namespace math {

struct VEC3
{
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

    f32 x;
    f32 y;
    f32 z;
};

} } // namespace nw::math

#endif // MATH_VEC3_H_
