#ifndef MATH_MTX44_H_
#define MATH_MTX44_H_

#include <math/math_VEC4.h>

namespace nw { namespace math {

struct MTX44
{
    union
    {
        f32  m[4][4];
        VEC4 v[4];
        f32  a[4 * 4];
    };
};

} } // namespace nw::math

#endif // MATH_MTX44_H_
