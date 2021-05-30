#ifndef MATH_VEC2_H_
#define MATH_VEC2_H_

#include <cmath>
#include <types.h>

namespace nw { namespace math {

struct VEC2
{
    void Normalize()
    {
        f32 inv_mag = 1.0f / sqrtf(x*x + y*y); // No division by zero check...
        x *= inv_mag;
        y *= inv_mag;
    }

    f32 x;
    f32 y;
};

} } // namespace nw::math

#endif // MATH_VEC2_H_
