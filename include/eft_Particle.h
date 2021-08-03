#ifndef EFT_PARTICLE_H_
#define EFT_PARTICLE_H_

#include <math/math_MTX34.h>
#include <math/math_VEC2.h>
#include <math/math_VEC3.h>
#include <ut/ut_Color4f.h>
#include <eft_Enum.h>

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
    math::VEC2 startDiff;
    math::VEC2 endDiff;
    s32 time2;
    s32 time3;
};
static_assert(sizeof(ScaleAnim) == 0x18, "ScaleAnim size mismatch");

struct TexUVParam // Actual name not known
{
    f32 rotate;
    math::VEC2 offset;
    math::VEC2 scroll;
    math::VEC2 scale;
};
static_assert(sizeof(TexUVParam) == 0x1C, "TexUVParam size mismatch");

struct PtclStripe;

struct ComplexEmitterParam // Actual name not known
{
    PtclStripe* stripe;
    f32 childEmitCounter;
    f32 childPreCalcCounter;
    f32 childEmitLostTime;
};
static_assert(sizeof(ComplexEmitterParam) == 0x10, "ComplexEmitterParam size mismatch");

struct ComplexEmitterData;
struct EmitterInstance;
struct PtclAttributeBuffer;
struct SimpleEmitterData;

struct PtclInstance
{
    f32 counter;
    f32 emitStartFrame;
    s32 counterS32;
    s32 lifespan;
    f32 randomF32;
    math::VEC3 pos;
    math::VEC3 posDiff;
    math::VEC3 velocity;
    math::VEC3 worldPos;
    math::VEC3 worldPosDiff;
    math::VEC3 rotation;
    math::VEC3 angularVelocity;
    union
    {
        f32 alpha[2];
        struct
        {
            f32 alpha0;
            f32 alpha1;
        };
    };
    math::VEC2 scale;
    union
    {
        ut::Color4f color[2];
        struct
        {
            ut::Color4f color0;
            ut::Color4f color1;
        };
    };
    f32 fluctuationAlpha;
    math::VEC2 fluctuationScale;
    math::MTX34 matrixRT;
    math::MTX34 matrixSRT;
    math::MTX34* pMatrixRT;
    math::MTX34* pMatrixSRT;
    PtclAttributeBuffer* ptclAttributeBuffer;
    const SimpleEmitterData* data;
    const ComplexEmitterData* cdata;
    ScaleAnim* scaleAnim;
    AlphaAnim* alphaAnim[2];
    ComplexEmitterParam* complexParam;
    PtclInstance* prev;
    PtclInstance* next;
    EmitterInstance* emitter;
    PtclType type;
    u32 randomU32;
    f32 _13C;
    u32 _unused;
    math::VEC4 randomVec4;
    u32 fieldCollisionCounter;
};
static_assert(sizeof(PtclInstance) == 0x158, "PtclInstance size mismatch");

struct PtclStripeSliceHistory // Actual name not known
{
    math::VEC3 pos;
    f32 scale;
    math::MTX34 emitterMatrixSRT;
    math::VEC3 outer;
    math::VEC3 dir;
};
static_assert(sizeof(PtclStripeSliceHistory) == 0x58, "PtclStripeSliceHistory size mismatch");

struct StripeUniformBlock;
struct StripeVertexBuffer;

struct PtclStripe
{
    PtclInstance* particle;
    u32 queueFront;
    u32 queueRear;
    PtclStripeSliceHistory queue[256];
    u32 queueCount;
    u32 groupID;
    const ComplexEmitterData* data;
    s32 counter;
    math::VEC3 currentSliceDir;
    math::VEC3 pos0;
    math::VEC3 pos1;
    PtclStripe* prev;
    PtclStripe* next;
    u32 drawFirstVertex;
    u32 numDraw;
    StripeUniformBlock* stripeUniformBlock;
    StripeUniformBlock* stripeUniformBlockCross;
    StripeVertexBuffer* stripeVertexBuffer;
    TexUVParam texAnimParam[2];
    u32 flags;
};
static_assert(sizeof(PtclStripe) == 0x5898, "PtclStripe size mismatch");

} } // namespace nw::eft

#endif // EFT_PARTICLE_H_
