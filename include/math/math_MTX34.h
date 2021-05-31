#ifndef MATH_MTX34_H_
#define MATH_MTX34_H_

#include <math/math_VEC4.h>

namespace nw { namespace math {

struct MTX34
{
    union
    {
        f32  m[3][4];
        VEC4 v[3];
        f32  a[3 * 4];
    };
};

} } // namespace nw::math

#endif // MATH_MTX34_H_
