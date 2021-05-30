#ifndef MATH_VEC3_H_
#define MATH_VEC3_H_

#include <cmath>
#include <types.h>

namespace nw { namespace math {

struct VEC3
{
    void Normalize()
    {
        f32 inv_mag = 1.0f / sqrtf(x*x + y*y + z*z); // No division by zero check...
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
