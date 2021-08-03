#ifndef EFT_UNIFORM_BLOCK_H_
#define EFT_UNIFORM_BLOCK_H_

#include <math/math_MTX44.h>
#include <math/math_VEC4.h>
#include <eft_Types.h>

namespace nw { namespace eft {

struct TexUvShiftAnimUbo
{
    math::VEC4 pm0;
    math::VEC4 pm1;
    math::VEC4 pm2;
    math::VEC4 pm3;
    math::VEC4 pm4;
};
static_assert(sizeof(TexUvShiftAnimUbo) == 0x50, "TexUvShiftAnimUbo size mismatch");

struct TexPtnAnimUbo
{
    math::VEC4 pm;
    s32 tbl[32]; // ivec4[8]
};
static_assert(sizeof(TexPtnAnimUbo) == 0x90, "TexPtnAnimUbo size mismatch");

struct EmitterStaticUniformBlock
{
    s32 flag[4]; // ivec4
    TexUvShiftAnimUbo shtAnim0;
    TexUvShiftAnimUbo shtAnim1;
    TexUvShiftAnimUbo shtAnim2;
    TexPtnAnimUbo ptnAnm0;
    TexPtnAnimUbo ptnAnm1;
    math::VEC4 flucX;
    math::VEC4 flucY;
    math::VEC4 rotBasis;
    math::VEC4 u_fresnelMinMax;
    math::VEC4 u_nearAlpha;
    math::VEC4 rotateVel;
    math::VEC4 rotateVelRandom;
    math::VEC4 alphaAnim0;
    math::VEC4 alphaAnim1;
    math::VEC4 scaleAnim0;
    math::VEC4 scaleAnim1;
    math::VEC4 scaleAnim2;
    math::VEC4 col0[8];
    math::VEC4 col1[8];
    math::VEC4 color0Anim;
    math::VEC4 color1Anim;
    math::VEC4 vectorParam;
    math::VEC4 gravityParam;
    math::VEC4 alphaAnim2;
    math::VEC4 fieldRandomParam;
    math::VEC4 fieldCollisionParam;
    math::VEC4 fieldPosAddParam;
    math::VEC4 fieldConvergenceParam;
    math::VEC4 fieldMagnetParam;
    math::VEC4 fieldSpinParam;
    math::VEC4 fieldCoordEmitter;
    math::VEC4 fieldCurlNoise0;
    math::VEC4 fieldCurlNoise1;
    math::VEC4 u_shaderParam;
    math::VEC4 u_shaderParamAnim0;
    math::VEC4 u_shaderParamAnim1;

    math::VEC4 pad[1]; // Pad the block to (multiple of) 256 bytes for hardware needs
};
static_assert(sizeof(EmitterStaticUniformBlock) == 0x500, "EmitterStaticUniformBlock size mismatch");

struct EmitterDynamicUniformBlock
{
    math::VEC4 emitterColor0;
    math::VEC4 emitterColor1;
    math::VEC4 emp0;
    math::VEC4 emp1;
    math::MTX44 emitterMatrix;
    math::MTX44 emitterMatrixRT;
};
static_assert(sizeof(EmitterDynamicUniformBlock) == 0xC0, "EmitterDynamicUniformBlock size mismatch");

struct ViewUniformBlock
{
    math::MTX44 viewMat;
    math::MTX44 projMat;
    math::MTX44 vpMat;
    math::MTX44 bldMat;
    math::VEC4 eyeVec;
    math::VEC4 eyePos;
    math::VEC4 depthBufferTexMat;
    math::VEC4 frameBufferTexMat;
    math::VEC4 viewParam;
};
static_assert(sizeof(ViewUniformBlock) == 0x150, "ViewUniformBlock size mismatch");

struct PtclAttributeBuffer
{
    math::VEC4 wldPos;
    math::VEC4 scl;
    math::VEC4 vec;
    math::VEC4 random;
    math::VEC4 rot;
    math::MTX34 emtMat;
    math::VEC4 color0;
    math::VEC4 color1;
    math::VEC4 wldPosDf;

    math::VEC4 pad[1];
};
static_assert(sizeof(PtclAttributeBuffer) == 0xC0, "PtclAttributeBuffer size mismatch");

struct PtclAttributeBufferGpu
{
    math::VEC4 wldPos;
    math::VEC4 scl;
    math::VEC4 vec;
    math::VEC4 random;
    math::VEC4 rot;
    math::MTX34 emtMat;
};
static_assert(sizeof(PtclAttributeBufferGpu) == 0x80, "PtclAttributeBufferGpu size mismatch");

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
