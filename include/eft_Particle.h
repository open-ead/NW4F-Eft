#ifndef EFT_PARTICLE_H_
#define EFT_PARTICLE_H_

#include <math/math_MTX34.h>
#include <math/math_VEC2.h>
#include <math/math_VEC3.h>
#include <eft_Types.h>

namespace nw { namespace eft {

struct AlphaAnim
{
    f32 startDiff;
    f32 endDiff;
    s32 time2;
    s32 time3;
};
static_assert(sizeof(AlphaAnim) == 0x10, "AlphaAnim size mismatch");

struct ScaleAnim // Name is not certain
{
    u8 _0[0x18];
};
static_assert(sizeof(ScaleAnim) == 0x18, "ScaleAnim size mismatch");

struct StripeUV // Actual name not known
{
    f32 rotate;
    math::VEC2 cameraOffset;
    math::VEC2 scroll;
    math::VEC2 scale;
};
static_assert(sizeof(StripeUV) == 0x1C, "StripeUV size mismatch");

struct EmitterInstance;
struct PtclAttributeBuffer;
struct PtclStripe;
struct SimpleEmitterData;

struct PtclInstance
{
    f32 counter;
    s32 lifespan;
    math::VEC3 pos;
    math::VEC3 posDiff;
    math::VEC3 velocity;
    math::VEC3 worldPos;
    math::VEC3 worldPosDiff;
    math::VEC3 rotation;
    math::VEC3 angularVelocity;
    f32 randomUnk;
    f32 alpha;
    AlphaAnim* alphaAnim;
    math::VEC2 scale;
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
static_assert(sizeof(PtclInstance) == 0x180, "PtclInstance size mismatch");

struct PtclStripeQueue // Actual name not known
{
    math::VEC3 pos;
    f32 scale;
    math::MTX34 emitterMatrixSRT;
    math::VEC3 outer;
    math::VEC3 dir;
};
static_assert(sizeof(PtclStripeQueue) == 0x58, "PtclStripeQueue size mismatch");

struct PtclStripe
{
    PtclInstance* particle;
    u32 queueFront;
    u32 queueRear;
    PtclStripeQueue queue[256];
    u32 queueCount;
    u32 groupID;
    SimpleEmitterData* data;
    s32 counter;
    math::MTX34 emitterMatrixSRT;
    u8 _584C[36];
    PtclStripe* prev;
    PtclStripe* next;
    u32 _5878;
    u32 _587C;
};
static_assert(sizeof(PtclStripe) == 0x5880, "PtclStripe size mismatch");

} } // namespace nw::eft

#endif // EFT_PARTICLE_H_
