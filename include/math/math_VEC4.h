#ifndef MATH_VEC4_H_
#define MATH_VEC4_H_

#include <cmath>
#include <types.h>

namespace nw { namespace math {

struct VEC4
{
    void Normalize()
    {
        f32 inv_mag = 1.0f / sqrtf(x*x + y*y + z*z + w*w); // No division by zero check...
        x *= inv_mag;
        y *= inv_mag;
        z *= inv_mag;
        w *= inv_mag;
    }

    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

} } // namespace nw::math

#endif // MATH_VEC4_H_
