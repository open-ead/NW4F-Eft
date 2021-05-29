#ifndef EFT_PARTICLE_H_
#define EFT_PARTICLE_H_

#include <math/math_MTX34.h>

namespace nw { namespace eft {

struct AlphaAnim
{
    f32 startDiff;
    f32 endDiff;
    s32 time2;
    s32 time3;
};

struct ScaleAnim // Name is not certain
{
    u8 _0[0x18];
};

struct StripeUV // Actual name not known
{
    f32 rotate;
    f32 cameraOffset[2]; // VEC2
    f32 scroll[2]; // VEC2
    f32 scale[2]; // VEC2
};

struct EmitterInstance;
struct PtclAttributeBuffer;
struct PtclStripe;
struct SimpleEmitterData;

struct PtclInstance
{
    f32 counter;
    s32 lifespan;
    f32 pos[3]; // VEC3
    f32 posDiff[3]; // VEC3
    f32 velocity[3]; // VEC3
    f32 worldPos[3]; // VEC3
    f32 worldPosDiff[3]; // VEC3
    f32 rotation[3]; // VEC3
    f32 angularVelocity[3]; // VEC3
    f32 randomUnk;
    f32 alpha;
    AlphaAnim* alphaAnim;
    f32 scale[2]; // VEC2
    ScaleAnim* scaleAnim;
    StripeUV stripUVs[2]; // For each texture
    f32 _AC;
    f32 _B0;
    f32 color[2][4];
    PtclAttributeBuffer* ptclAttributeBuffer;
    math::MTX34 matrixRT;
    math::MTX34 matrixSRT;
    math::MTX34* pMatrixRT;
    math::MTX34* pMatrixSRT;
    u32 _140;
    SimpleEmitterData* data;
    PtclInstance* prev;
    PtclInstance* next;
    EmitterInstance* emitter;
    PtclStripe* stripe;
    u32 particleType;
    u32 randomUnk2;
    u8 _160[32];
};

} } // namespace nw::eft

#endif // EFT_PARTICLE_H_
