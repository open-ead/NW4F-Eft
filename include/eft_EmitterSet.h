#ifndef EFT_EMITTER_SET_H_
#define EFT_EMITTER_SET_H_

#include <math/math_MTX34.h>

namespace nw { namespace eft {

struct EmitterInstance;

class EmitterController
{
public:
    f32 _0;
    f32 _4;
    f32 _8;
    u32 _C;
    EmitterInstance* emitter;
};

class System;

class EmitterSet
{
public:
    EmitterSet()
        : numEmitter(0)
        , numEmitterMax(0)
        , createID(0)
        , _198(0)
        , _27C(NULL)
    {
        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
        color[3] = 1.0f;

        _26C[0] = 0.0f;
        _26C[1] = 0.0f;
        _26C[2] = 0.0f;

        for (u32 i = 0; i < 16; i++)
            emitters[i] = NULL;
    }

    System* system;
    u32 numEmitter;
    u32 numEmitterMax;
    u32 createID;
    EmitterInstance* emitters[16];
    EmitterController controllers[16];
    u32 resourceID;
    u32 emitterSetID;
    u32 _198;
    u32 groupID;
    u32 _1A0;
    math::MTX34 matrixSRT;
    math::MTX34 matrixRT;
    f32 _204[3]; // VEC3
    f32 _210[2]; // VEC2
    f32 _218[2]; // VEC2
    f32 _220[2]; // VEC2
    f32 _228[3]; // VEC3
    f32 color[4]; // Color4f
    f32 _244;
    f32 _248;
    f32 _24C;
    f32 _250[3]; // VEC3
    f32 _25C[3]; // VEC3
    u32 _268;
    f32 _26C[3]; // VEC3
    u32 _278;
    f32* _27C; // VEC3*
    EmitterSet* next;
    EmitterSet* prev;
    u8 doFade;
    u8 _289;
    u8 noCalc;
    u8 noDraw;
    u8 _28C[8];
};

} } // namespace nw::eft

#endif // EFT_EMITTER_SET_H_
