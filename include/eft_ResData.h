#ifndef EFT_RES_DATA_H_
#define EFT_RES_DATA_H_

#include <math/math_VEC2.h>
#include <math/math_VEC3.h>
#include <ut/ut_Color4f.h>
#include <eft_Enum.h>

#define INCLUDE_CAFE
#include <eft_Types.h>

namespace nw { namespace eft {

struct TextureRes
{
    u16 width;
    u16 height;
    GX2TileMode tileMode;
    u32 swizzle;
    u8 _C[8];
    u8 wrapMode;
    u8 filterMode;
    u32 numMips;
    u32 compSel;
    u32 mipOffset[13];
    f32 maxLOD;
    f32 biasLOD;
    TextureResFormat originalTexFormat;
    u32 originalTexDataOffs;
    u32 originalTexDataSize;
    TextureResFormat cafeTexFormat;
    u32 cafeTexDataSize;
    u32 cafeTexDataOffs;
    u32 initialized;
    GX2Texture gx2Texture;
};
static_assert(sizeof(TextureRes) == 0x114, "TextureRes size mismatch");

struct EmitterData // Actual name not known
{
    EmitterType type;
    u32 flags;
    u32 seed;
    u32 userData;
    u32 _10;
    f32 _14[8];
    CustomActionCallBackID callbackID;
    u32 nameOffs;
    const char* name;
    TextureRes textures[2];
    void* keyAnimArray;
    u32 keyAnimArrayOffs;
    u32 keyAnimArraySize;
    u32 _274;
    u32 _278;
    u32 primitiveIdx;
};
static_assert(sizeof(EmitterData) == 0x280, "EmitterData size mismatch");

struct TextureEmitterData
{
    bool hasTexPtnAnim;
    bool texPtnAnimRandStart;
    bool texPtnAnimLimitStart;
    u8 idxDiv;
    u8 _4; // unused
    u8 idxRand;
    u8 _6[2]; // unused
    s16 texPtnFreq;
    s16 texPtnAnimDataSize;
    u8 texPtnAnimData[32];
    u32 _2C; // unused
    math::VEC2 uvScaleInit;
    u32 uvShiftAnimMode;
    math::VEC2 texIncScroll;
    math::VEC2 texInitScroll;
    math::VEC2 texInitScrollRandom;
    math::VEC2 texIncScale;
    math::VEC2 texInitScale;
    math::VEC2 texInitScaleRandom;
    f32 texIncRotate;
    f32 texInitRotate;
    f32 texInitRotateRandom;
};
static_assert(sizeof(TextureEmitterData) == 0x78, "TextureEmitterData size mismatch");

struct anim3v4Key
{
    f32 startValue;
    f32 startDiff;
    f32 endDiff;
    s32 time2;
    s32 time3;
};
static_assert(sizeof(anim3v4Key) == 0x14, "anim3v4Key size mismatch");

struct SimpleEmitterData : EmitterData
{
    u8 _280[0x28C - 0x280];
    u32 rotationMode;
    PtclFollowType ptclFollowType;
    u32 fragmentColorMode;
    u32 fragmentAlphaMode;
    s32 _29C;
    u8 _2A0[0x308 - 0x2A0];
    math::VEC3 emitterScale;
    math::VEC3 emitterRotate;
    math::VEC3 emitterTranslate;
    math::VEC3 emitterRotateRandom;
    math::VEC3 emitterTranslateRandom;
    u8 _344[0x37C - 0x344];
    math::VEC3 emissionShapeScale;
    ut::Color4f emitterColor0;
    ut::Color4f emitterColor1;
    f32 emitterAlpha;
    u8 _3AC[0x3BC - 0x3AC];
    f32 emissionRate;
    s32 startFrame;
    s32 endFrame;
    s32 emitInterval;
    s32 emitIntervalRandom;
    f32 ptclVelocityMag;
    f32 emitterVelocityMag;
    u8 _3D8[0x3F8 - 0x3D8];
    f32 airResist;
    math::VEC3 gravity;
    f32 _408;
    f32 _40C;
    s32 ptclMaxLifespan;
    s32 ptclLifespanRandom;
    MeshType meshType;
    VertexTransformMode vertexTransformMode;
    math::VEC2 rotBasis;
    f32 cameraOffset;
    TextureEmitterData texAnimParam[2];
    ColorSourceType ptclColor0Src;
    ColorSourceType ptclColor1Src;
    ut::Color4f ptclColor0Tbl[3];
    ut::Color4f ptclColor1Tbl[3];
    u8 _584[0x5B0 - 0x584];
    anim3v4Key alphaAnim;
    FragmentComposite textureColorBlend;
    FragmentComposite primitiveColorBlend;
    FragmentComposite textureAlphaBlend;
    FragmentComposite primitiveAlphaBlend;
    s32 scaleAnimTime2;
    s32 scaleAnimTime3;
    math::VEC2 _5DC;
    math::VEC2 _5E4;
    math::VEC2 _5EC;
    math::VEC2 _5F4;
    f32 _5FC;
    math::VEC3 ptclRotate;
    math::VEC3 ptclRotateRandom;
    math::VEC3 angularVelocity;
    math::VEC3 angularVelocityRandom;
    f32 rotInertia;
    f32 fadeAlphaStep;
    u8 fragmentShaderMode;
    u8 shaderUserSetting;
    bool fragmentSoftEdge;
    bool refractionApplyAlpha;
    math::VEC2 shaderParam01;
    f32 fragmentSoftEdgeFadeDist;
    f32 fragmentSoftEdgeVolume;
    char userMacro1[16];
    char userMacro2[16];
    u32 shaderUserFlag;
    u32 shaderUserSwitchFlag;
    u8 _674[0x6F4 - 0x674];
};
static_assert(sizeof(SimpleEmitterData) == 0x6F4, "SimpleEmitterData size mismatch");

struct ComplexEmitterData : SimpleEmitterData
{
    u32 childFlags;
    u16 _6F8;
    u16 _6FA;
    u16 stripeFlags;
    u8 _6FE[2];
    u16 childDataOffs;
    u16 _702;
    u16 _704;
    u16 stripeDataOffs;
    u32 _708;
};
static_assert(sizeof(ComplexEmitterData) == 0x70C, "ComplexEmitterData size mismatch");

struct ChildData
{
    u8 _0[0x28];
    u32 _28;
    u32 _2C;
    u32 primitiveIdx;
    f32 _34;
    u8 _38[4];
    MeshType meshType;
    VertexTransformMode vertexTransformMode;
    u8 _44[4];
    TextureRes texture;
    u8 _15C[0x17C - 0x15C];
    FragmentComposite primitiveColorBlend;
    FragmentComposite primitiveAlphaBlend;
    u8 _184[0x1A0 - 0x184];
    u32 rotationMode;
    u8 _1A4[0x1D8 - 0x1A4];
    math::VEC2 rotBasis;
    u8 _1E0[0x204 - 0x1E0];
    math::VEC2 uvScaleInit;
    u8 _20C[0x234 - 0x20C];
    u32 fragmentColorMode;
    u32 fragmentAlphaMode;
    f32 _23C;
    u8 fragmentShaderMode;
    u8 shaderUserSetting;
    bool fragmentSoftEdge;
    bool refractionApplyAlpha;
    math::VEC2 shaderParam01;
    f32 fragmentSoftEdgeFadeDist;
    f32 fragmentSoftEdgeVolume;
    char userMacro1[16];
    char userMacro2[16];
    u32 shaderUserFlag;
    u32 shaderUserSwitchFlag;
    u8 _27C[0x2FC - 0x27C];
};
static_assert(sizeof(ChildData) == 0x2FC, "ChildData size mismatch");

struct EmitterReference // Actual name not known
{
    u32 dataOffs;
    EmitterData* data;
};
static_assert(sizeof(EmitterReference) == 8, "EmitterReference size mismatch");

struct EmitterSetData // Actual name not known
{
    u32 userData;
    u32 _4;
    u32 nameOffs;
    const char* name;
    s32 numEmitter;
    u32 emitterRefOffs;
    EmitterReference* emitterRef;
};
static_assert(sizeof(EmitterSetData) == 0x1C, "EmitterSetData size mismatch");

struct Header // Actual name not known
{
    char magic[4];
    u32 version;
    s32 numEmitterSet;
    u32 _C; // Unused
    u32 strTblOffs;
    u32 textureDataTblOffs;
    u32 textureDataTblSize;
    u32 shaderTblOffs;
    u32 shaderTblSize;
    u32 keyAnimArrayTblOffs;
    u32 keyAnimArrayTblSize;
    u32 primitiveTblOffs;
    u32 primitiveTblSize;
    u32 _34; // Unused
    u32 _38; // Unused
    u32 _3C; // Unused
};
static_assert(sizeof(Header) == 0x40, "Header size mismatch");

struct PrimitiveAttrib // Actual name not known
{
    u32 count;
    u32 size;
    u32 bufferOffs;
    u32 bufferSize;
};
static_assert(sizeof(PrimitiveAttrib) == 0x10, "PrimitiveAttrib size mismatch");

struct PrimitiveData // Actual name not known
{
    PrimitiveAttrib pos;
    PrimitiveAttrib normal;
    PrimitiveAttrib color;
    PrimitiveAttrib texCoord;
    PrimitiveAttrib index;
};
static_assert(sizeof(PrimitiveData) == 0x50, "PrimitiveData size mismatch");

struct PrimitiveTable // Actual name not known
{
    u32 numPrimitive;
    u32 size;
    u32 primitiveOffs;
};
static_assert(sizeof(PrimitiveTable) == 0xC, "PrimitiveTable size mismatch");

struct StripeData
{
    u32 type;
    u8 _4[0x34 - 0x4];
};
static_assert(sizeof(StripeData) == 0x34, "StripeData size mismatch");

} } // namespace nw::eft

#endif // EFT_RES_DATA_H_
