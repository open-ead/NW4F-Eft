#ifndef MATH_MTX44_H_
#define MATH_MTX44_H_

#include <math/math_VEC4.h>

namespace nw { namespace math {

struct MTX44
{
    MTX44() { }
    MTX44(const MTX44& other) { Copy(this, &other); }

    MTX44(
        f32 _00, f32 _01, f32 _02, f32 _03,
        f32 _10, f32 _11, f32 _12, f32 _13,
        f32 _20, f32 _21, f32 _22, f32 _23,
        f32 _30, f32 _31, f32 _32, f32 _33
    )
    {
        m[0][0] = _00;
        m[0][1] = _01;
        m[0][2] = _02;
        m[0][3] = _03;

        m[1][0] = _10;
        m[1][1] = _11;
        m[1][2] = _12;
        m[1][3] = _13;

        m[2][0] = _20;
        m[2][1] = _21;
        m[2][2] = _22;
        m[2][3] = _23;

        m[3][0] = _30;
        m[3][1] = _31;
        m[3][2] = _32;
        m[3][3] = _33;
    }

    static const MTX44& Identity()
    {
        static const MTX44 identity(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        return identity;
    }

    static MTX44* Copy(MTX44* dst, const MTX44* src)
    {
        if (src != dst)
        {
            VEC4* vDst = dst->v;
            const VEC4* vSrc = src->v;

            for (s32 i = 0; i < 4; i++)
                *vDst++ = *vSrc++;
        }

        return dst;
    }

    MTX44& operator=(const MTX44& other)
    {
        return *Copy(this, &other);
    }

    union
    {
        f32  m[4][4];
        VEC4 v[4];
        f32  a[4 * 4];
    };
};

} } // namespace nw::math

#endif // MATH_MTX44_H_
