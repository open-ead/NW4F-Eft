#ifndef MATH_VEC2_H_
#define MATH_VEC2_H_

#include <cmath>
#include <types.h>

namespace nw { namespace math {

struct VEC2
{
    f32 MagnitudeSquare() const
    {
        return x*x + y*y;
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
    }

    f32 x;
    f32 y;
};

} } // namespace nw::math

#endif // MATH_VEC2_H_
