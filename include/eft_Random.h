#ifndef EFT_RANDOM_H_
#define EFT_RANDOM_H_

#include <math/math_VEC3.h>
#include <eft_Types.h>

namespace nw { namespace eft {

class Random
{
// https://github.com/open-ead/sead/blob/master/include/random/seadRandom.h
// https://github.com/open-ead/sead/blob/master/modules/src/random/seadRandom.cpp

public:
    Random()
    {
        Init();
    }

    Random(u32 seed)
    {
        Init(seed);
    }

    Random(u32 seed_x, u32 seed_y, u32 seed_z, u32 seed_w)
    {
        Init(seed_x, seed_y, seed_z, seed_w);
    }

    void Init()
    {
        Init(0);
    }

    void Init(u32 seed)
    {
        static const u32 a = 0x6C078965;
        mX = a * (seed ^ (seed >> 30u)) + 1;
        mY = a * (mX ^ (mX >> 30u)) + 2;
        mZ = a * (mY ^ (mY >> 30u)) + 3;
        mW = a * (mZ ^ (mZ >> 30u)) + 4;
    }

    void Init(u32 seed_x, u32 seed_y, u32 seed_z, u32 seed_w)
    {
        if ((seed_x | seed_y | seed_z | seed_w) == 0) // seeds must not be all zero.
            return Init(0);

        mX = seed_x;
        mY = seed_y;
        mZ = seed_z;
        mW = seed_w;
    }

    u32 GetU32()
    {
        u32 x = mX ^ (mX << 11u);
        mX = mY;
        mY = mZ;
        mZ = mW;
        mW = mW ^ (mW >> 19u) ^ x ^ (x >> 8u);
        return mW;
    }

    u32 GetU32(u32 max)
    {
        return GetU32() * u64(max) >> 32u;
    }

    s32 GetS32Range(s32 a, s32 b)
    {
        return GetU32(b - a) + a;
    }

    f32 GetF32()
    {
        return GetU32() * (1.0f / 4294967296.0f);
    }

    f32 GetF32(f32 max)
    {
        return GetF32() * max;
    }

    f32 GetF32Range(f32 a, f32 b)
    {
        return GetF32(b - a) + a;
    }

private:
    u32 mX;
    u32 mY;
    u32 mZ;
    u32 mW;
};

class Heap;

class PtclRandom
{
public:
    PtclRandom();

    static void Initialize(Heap* heap);
    static Random* GetGlobalRandom();

    static Random gRandom;
    static math::VEC3* mVec3Tbl;
    static math::VEC3* mNormalizedVec3Tbl;

    u16 randomVec3Idx;
    u16 randomNormVec3Idx;
    u32 val;
};

} } // namespace nw::eft

#endif // EFT_RANDOM_H_
