#ifndef EFT_EMITTER_SET_H_
#define EFT_EMITTER_SET_H_

#include <math/math_MTX34.h>
#include <math/math_VEC2.h>
#include <math/math_VEC3.h>
#include <ut/ut_Color4f.h>
#include <eft_Types.h>

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
static_assert(sizeof(EmitterController) == 0x14, "EmitterController size mismatch");

class System;

class EmitterSet
{
public:
    EmitterSet()
        : numEmitter(0)
        , numEmitterAtCreate(0)
        , createID(0)
        , userData(0)
        , _27C(NULL)
    {
        color.v = (math::VEC4){ 1.0f, 1.0f, 1.0f, 1.0f };
        _26C    = (math::VEC3){ 0.0f, 0.0f, 0.0f };

        for (u32 i = 0; i < 16u; i++)
            emitters[i] = NULL;
    }

    System* system;
    u32 numEmitter;
    u32 numEmitterAtCreate;
    u32 createID;
    EmitterInstance* emitters[16];
    EmitterController controllers[16];
    u32 resourceID;
    u32 emitterSetID;
    u32 userData;
    u32 groupID;
    u32 _1A0;
    math::MTX34 matrixSRT;
    math::MTX34 matrixRT;
    math::VEC3 _204;
    math::VEC2 _210;
    math::VEC2 _218;
    math::VEC2 _220;
    math::VEC3 _228;
    ut::Color4f color;
    f32 _244;
    f32 _248;
    f32 _24C;
    math::VEC3 _250;
    math::VEC3 _25C;
    u32 _268;
    math::VEC3 _26C;
    u32 _278;
    math::VEC3* _27C;
    EmitterSet* next;
    EmitterSet* prev;
    u8 doFade;
    u8 _289;
    u8 noCalc;
    u8 noDraw;
    u8 _28C;
    u8 _28D;
    u8 _28E[6];
};
static_assert(sizeof(EmitterSet) == 0x294, "EmitterSet size mismatch");

} } // namespace nw::eft

#endif // EFT_EMITTER_SET_H_
