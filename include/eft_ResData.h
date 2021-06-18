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
    bool texPtnAnimClamp;
    u8 texPtnAnimIdxDiv;
    u8 _4; // unused
    u8 texPtnAnimNum;
    u8 _6[2]; // unused
    s16 texPtnAnimPeriod;
    s16 texPtnAnimUsedSize;
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
    u8 _280;
    u8 _281;
    u8 _282;
    u8 _283;
    u8 _284;
    u8 _285;
    u8 _286;
    u8 _287;
    u8 displayParent;
    u8 _289;
    u8 _28A;
    VertexRotationMode rotationMode;
    PtclFollowType ptclFollowType;
    u32 fragmentColorMode;
    u32 fragmentAlphaMode;
    s32 _29C;
    DisplaySideType displaySideType;
    u8 _2A4[0x308 - 0x2A4];
    math::VEC3 emitterScale;
    math::VEC3 emitterRotate;
    math::VEC3 emitterTranslate;
    math::VEC3 emitterRotateRandom;
    math::VEC3 emitterTranslateRandom;
    BlendType blendType;
    ZBufATestType zBufATestType;
    u32 _34C;
    math::VEC3 _350;
    u32 _35C;
    u32 _360;
    f32 _364;
    f32 _368;
    math::VEC3 _36C;
    f32 _378;
    math::VEC3 emissionShapeScale;
    ut::Color4f emitterColor0;
    ut::Color4f emitterColor1;
    f32 emitterAlpha;
    f32 _3AC;
    f32 _3B0;
    f32 _3B4;
    f32 _3B8;
    f32 emissionRate;
    s32 startFrame;
    s32 endFrame;
    s32 emitInterval;
    s32 emitIntervalRandom;
    f32 ptclVelocityMag;
    f32 emitterVelocityMag;
    f32 _3D8;
    math::VEC3 _3DC;
    f32 _3E8;
    math::VEC3 _3EC;
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
    ColorSourceType ptclColorSrc[2];
    ut::Color4f ptclColorTbl[2][3];
    s32 colorTime2[2];
    s32 colorTime3[2];
    s32 colorTime4[2];
    s32 colorNumRepetition[2];
    s32 colorRandomStart[2];
    f32 colorScaleFactor;
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
    u16 fieldFlags;
    u16 fluctuationFlags;
    u16 stripeFlags;
    u8 _6FE[2];
    u16 childDataOffs;
    u16 fieldDataOffs;
    u16 fluctuationDataOffs;
    u16 stripeDataOffs;
    u32 dataSize;
};
static_assert(sizeof(ComplexEmitterData) == 0x70C, "ComplexEmitterData size mismatch");

struct ChildData
{
    s32 numChildParticles;
    s32 _4;
    s32 ptclMaxLifespan;
    s32 _C;
    f32 velocityScaleFactor;
    f32 _14;
    math::VEC3 _18;
    f32 _24;
    u32 _28;
    u32 _2C;
    u32 primitiveIdx;
    f32 _34;
    BlendType blendType;
    MeshType meshType;
    VertexTransformMode vertexTransformMode;
    ZBufATestType zBufATestType;
    TextureRes texture;
    DisplaySideType displaySideType;
    math::VEC3 _160;
    math::VEC3 _16C;
    f32 _178;
    FragmentComposite primitiveColorBlend;
    FragmentComposite primitiveAlphaBlend;
    f32 _184;
    f32 _188;
    f32 _18C;
    f32 _190;
    math::VEC2 _194;
    f32 _19C;
    VertexRotationMode rotationMode;
    math::VEC3 _1A4;
    math::VEC3 _1B0;
    math::VEC3 _1BC;
    math::VEC3 _1C8;
    f32 _1D4;
    math::VEC2 rotBasis;
    math::VEC3 _1E0;
    s32 _1EC;
    s32 _1F0;
    s32 _1F4;
    math::VEC2 _1F8;
    u16 texPtnAnimNum;
    u8 texPtnAnimIdxDiv;
    math::VEC2 uvScaleInit;
    u8 texPtnAnimData[32];
    s16 texPtnAnimPeriod;
    s16 texPtnAnimUsedSize;
    bool texPtnAnimClamp;
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

struct FieldRandomData
{
    s32 period;
    math::VEC3 randomVelScale;
};
static_assert(sizeof(FieldRandomData) == 0x10, "FieldRandomData size mismatch");

struct FieldMagnetData
{
    f32 strength;
    math::VEC3 pos;
    u32 flags;
};
static_assert(sizeof(FieldMagnetData) == 0x14, "FieldMagnetData size mismatch");

struct FieldSpinData
{
    s32 angle;
    u32 axis;
    f32 diffusionVel;
};
static_assert(sizeof(FieldSpinData) == 0xC, "FieldSpinData size mismatch");

struct FieldCollisionData
{
    u16 collisionType;
    u16 coordSystem;
    f32 y;
    f32 friction;
};
static_assert(sizeof(FieldCollisionData) == 0xC, "FieldCollisionData size mismatch");

struct FieldConvergenceData
{
    math::VEC3 pos;
    f32 strength;
};
static_assert(sizeof(FieldConvergenceData) == 0x10, "FieldConvergenceData size mismatch");

struct FieldPosAddData
{
    math::VEC3 posAdd;
};
static_assert(sizeof(FieldPosAddData) == 0xC, "FieldPosAddData size mismatch");

struct FluctuationData
{
    f32 amplitude;
    f32 frequency;
    u32 enableRandom;
};
static_assert(sizeof(FluctuationData) == 0xC, "FluctuationData size mismatch");

struct StripeData
{
    u32 type;
    u32 crossType;
    u32 connectionType;
    u32 _C;
    u32 queueCount;
    u32 numConnections;
    f32 alphaStart;
    f32 alphaEnd;
    math::VEC2 _20;
    u32 _28;
    f32 _2C;
    f32 _30;
};
static_assert(sizeof(StripeData) == 0x34, "StripeData size mismatch");

struct EmitterTblData
{
    u32 dataOffs;
    EmitterData* data;
};
static_assert(sizeof(EmitterTblData) == 8, "EmitterTblData size mismatch");

struct EmitterSetData // Actual name not known
{
    u32 userData;
    u32 _4;
    u32 nameOffs;
    const char* name;
    s32 numEmitter;
    u32 emitterRefOffs;
    EmitterTblData* emitterRef;
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

} } // namespace nw::eft

#endif // EFT_RES_DATA_H_
