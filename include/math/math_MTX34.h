#ifndef MATH_MTX34_H_
#define MATH_MTX34_H_

#include <math/math_VEC4.h>

namespace nw { namespace math {

struct MTX34
{
    MTX34() { }
    MTX34(const MTX34& other) { Copy(this, &other); }

    MTX34(
        f32 _00, f32 _01, f32 _02, f32 _03,
        f32 _10, f32 _11, f32 _12, f32 _13,
        f32 _20, f32 _21, f32 _22, f32 _23
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
    }

    static const MTX34& Identity()
    {
        static const MTX34 identity(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f
        );
        return identity;
    }

    static MTX34* Copy(MTX34* dst, const MTX34* src)
    {
        if (src != dst)
            *dst = *src;

        return dst;
    }

    static MTX34* Transpose(MTX34* dst, const MTX34* src)
    {
        if (src == dst)
        {
            MTX34 temp;
            temp.m[0][0] = src->m[0][0];
            temp.m[0][1] = src->m[1][0];
            temp.m[0][2] = src->m[2][0];
            temp.m[0][3] = 0.0f;
            temp.m[1][0] = src->m[0][1];
            temp.m[1][1] = src->m[1][1];
            temp.m[1][2] = src->m[2][1];
            temp.m[1][3] = 0.0f;
            temp.m[2][0] = src->m[0][2];
            temp.m[2][1] = src->m[1][2];
            temp.m[2][2] = src->m[2][2];
            temp.m[2][3] = 0.0f;
            Copy(dst, &temp);
        }
        else
        {
            dst->m[0][0] = src->m[0][0];
            dst->m[0][1] = src->m[1][0];
            dst->m[0][2] = src->m[2][0];
            dst->m[0][3] = 0.0f;
            dst->m[1][0] = src->m[0][1];
            dst->m[1][1] = src->m[1][1];
            dst->m[1][2] = src->m[2][1];
            dst->m[1][3] = 0.0f;
            dst->m[2][0] = src->m[0][2];
            dst->m[2][1] = src->m[1][2];
            dst->m[2][2] = src->m[2][2];
            dst->m[2][3] = 0.0f;
        }
        return dst;
    }

    union
    {
        f32  m[3][4];
        VEC4 v[3];
        f32  a[3 * 4];
    };
};

} } // namespace nw::math

#endif // MATH_MTX34_H_
