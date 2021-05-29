#ifndef MATH_TRIANGULAR_H
#define MATH_TRIANGULAR_H

#include <math/math_Constant.h>

namespace nw { namespace math { namespace internal {

// Based on sead
struct SinCosSample
{
    f32 sin_val;
    f32 sin_delta;
    f32 cos_val;
    f32 cos_delta;
};

extern const SinCosSample gSinCosTbl[0x100+1];

} // namespace nw::math::internal

inline void sinCosIdx(f32* sin_val, f32* cos_val, u32 angle)
{
    u32 idx = (angle >> 24) & 0xff;
    f32 del = static_cast<f32>(angle & 0xffffff) / 0x1000000;
    const internal::SinCosSample& sample = internal::gSinCosTbl[idx];

    /*if (sin_val != NULL)*/ *sin_val = sample.sin_val + sample.sin_delta * del;
    /*if (cos_val != NULL)*/ *cos_val = sample.cos_val + sample.cos_delta * del;
}

inline f32 sinIdx(u32 angle)
{
    // Does not match if using sinCosIdx
    // f32 sin_val; sinCosIdx(&sin_val, NULL, angle);
    // return sin_val;

    u32 idx = (angle >> 24) & 0xff;
    f32 del = static_cast<f32>(angle & 0xffffff) / 0x1000000;
    const internal::SinCosSample& sample = internal::gSinCosTbl[idx];

    return sample.sin_val + sample.sin_delta * del;
}

inline f32 cosIdx(u32 angle)
{
    // Does not match if using sinCosIdx
    // f32 cos_val; sinCosIdx(NULL, &cos_val, angle);
    // return cos_val;

    u32 idx = (angle >> 24) & 0xff;
    f32 del = static_cast<f32>(angle & 0xffffff) / 0x1000000;
    const internal::SinCosSample& sample = internal::gSinCosTbl[idx];

    return sample.cos_val + sample.cos_delta * del;
}

inline u32 rad2Idx(f32 rad)
{
    return static_cast<s64>(rad * (I_HALF_ROUND_IDX / F_PI));
}

inline void sinCosRad(f32* sin_val, f32* cos_val, f32 rad)
{
    sinCosIdx(sin_val, cos_val, rad2Idx(rad));
}

inline f32 sinRad(f32 rad)
{
    // Does not match if using sinCosRad
    // f32 sin_val; sinCosRad(&sin_val, NULL, rad);
    // return sin_val;

    return sinIdx(rad2Idx(rad));
}

inline f32 cosRad(f32 rad)
{
    // Does not match if using sinCosRad
    // f32 cos_val; sinCosRad(NULL, &cos_val, rad);
    // return cos_val;

    return cosIdx(rad2Idx(rad));
}

} } // namespace nw::math

#endif // MATH_TRIANGULAR_H
