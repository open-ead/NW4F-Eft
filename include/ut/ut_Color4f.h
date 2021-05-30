#ifndef UT_COLOR4F_H_
#define UT_COLOR4F_H_

#include <math/math_VEC4.h>

namespace nw { namespace ut {

struct Color4f
{
    union
    {
        math::VEC4 v;
        struct
        {
            f32 r;
            f32 g;
            f32 b;
            f32 a;
        };
    };
};

} }

#endif // UT_COLOR4F_H_
