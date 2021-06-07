#ifndef EFT_UNIFORM_BLOCK_H_
#define EFT_UNIFORM_BLOCK_H_

#include <math/math_MTX44.h>
#include <math/math_VEC4.h>
#include <eft_Types.h>

namespace nw { namespace eft {

struct EmitterStaticUniformBlock
{
    math::VEC4 uvScaleInit;
    math::VEC4 rotBasis;
    math::VEC4 shaderParam;

    math::VEC4 pad[0x10-3]; // Pad the block to 256 bytes for hardware needs
};
static_assert(sizeof(EmitterStaticUniformBlock) == 0x100, "EmitterStaticUniformBlock size mismatch");

struct ViewUniformBlock
{
    math::MTX44 viewMat;
    math::MTX44 vpMat;
    math::MTX44 bldMat;
    math::VEC4 eyeVec;
    math::VEC4 eyePos;
    math::VEC4 depthBufferTexMat;
    math::VEC4 frameBufferTexMat;
    math::VEC4 viewParam;
};
static_assert(sizeof(ViewUniformBlock) == 0x110, "ViewUniformBlock size mismatch");

struct PtclAttributeBuffer
{
    u8 _0[0xB0];
};
static_assert(sizeof(PtclAttributeBuffer) == 0xB0, "PtclAttributeBuffer size mismatch");

struct StripeUniformBlock
{
    math::VEC4 stParam;
    math::VEC4 uvScrollAnim;
    math::VEC4 uvScaleRotateAnim0;
    math::VEC4 uvScaleRotateAnim1;
    math::VEC4 vtxColor0;
    math::VEC4 vtxColor1;
    math::MTX44 emitterMat;
};
static_assert(sizeof(StripeUniformBlock) == 0xA0, "StripeUniformBlock size mismatch");

struct StripeVertexBuffer
{
    math::VEC4 pos;
    math::VEC4 outer;
    math::VEC4 texCoord;
    math::VEC4 dir;
};
static_assert(sizeof(StripeVertexBuffer) == 0x40, "StripeVertexBuffer size mismatch");

} } // namespace nw::eft

#endif // EFT_UNIFORM_BLOCK_H_
