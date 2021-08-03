#ifndef EFT_RES_DATA_H_
#define EFT_RES_DATA_H_

#include <math/math_MTX34.h>
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
    u8 wrapMode;
    u8 filterMode;
    u8 depth;
    u32 numMips;
    u32 compSel;
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
static_assert(sizeof(TextureRes) == 0xD8, "TextureRes size mismatch");

struct KeyFrameAnimArray
{
    char magic[4];
    u32 numAnim;
};
static_assert(sizeof(KeyFrameAnimArray) == 8, "KeyFrameAnimArray size mismatch");

struct EmitterKeyAnimArray
{
    KeyFrameAnimArray* ptr;
    u32 offset;
    u32 size;
};
static_assert(sizeof(EmitterKeyAnimArray) == 0xC, "EmitterKeyAnimArray size mismatch");

struct EmitterPrimitive
{
    u8 _unusedPad[8];
    u32 idx;
};
static_assert(sizeof(EmitterPrimitive) == 0xC, "EmitterPrimitive size mismatch");

struct EmitterShaderParam
{
    u32 count;
    u32 offset;
    f32* ptr;
};
static_assert(sizeof(EmitterShaderParam) == 0xC, "EmitterShaderParam size mismatch");

struct EmitterData // Actual name not known
{
    EmitterType type;
    u32 flags;
    u32 seed;
    u32 userData;
    u8 _unusedPad0[36];
    CustomActionCallBackID callbackID;
    u32 nameOffs;
    const char* name;
    TextureRes textures[3];
    EmitterKeyAnimArray keyAnimArray;
    EmitterPrimitive primitive;
    EmitterPrimitive volumePrimitive;
    EmitterShaderParam shaderParam;
};
static_assert(sizeof(EmitterData) == 0x2F8, "EmitterData size mismatch");

struct TextureEmitterData
{
    bool hasTexPtnAnim;
    bool texPtnAnimRandStart;
    bool texPtnAnimClamp;
    u8 texPtnAnimIdxDiv[2];
    u8 texPtnAnimNum;
    u8 texInvURandom;
    u8 texInvVRandom;
    bool texSurfaceNoCrossFade;
    bool texUseSphereUV;
    u8 _unusedPad[2];
    u8 texPtnAnimData[32];
    u32 texPtnAnimMode;
    s16 texPtnAnimPeriod;
    s16 texPtnAnimUsedSize;
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
static_assert(sizeof(TextureEmitterData) == 0x7C, "TextureEmitterData size mismatch");

struct anim3v4Key
{
    f32 startValue;
    f32 startDiff;
    f32 endDiff;
    f32 time2;
    f32 time3;
};
static_assert(sizeof(anim3v4Key) == 0x14, "anim3v4Key size mismatch");

struct SimpleEmitterData : EmitterData
{
    u8 transformGravity;
    u8 noEmitAtFade;
    u8 displayParent;
    u8 emitSameDistance;
    u8 rotateDirRandomX;
    u8 rotateDirRandomY;
    u8 rotateDirRandomZ;
    u8 oneTime;
    u8 sphereUseLatitude;
    u8 updateEmitterInfoByEmit;
    u8 sphereDivTblIdx;
    u8 arcStartAngleRandom;
    u8 primitiveScaleYToZ;
    u8 forceRandomType0;
    u32 particleBehaviorFlg;
    VertexRotationMode rotationMode;
    PtclFollowType ptclFollowType;
    u32 fragmentColorMode;
    u32 fragmentAlphaMode;
    s32 _bitForUnusedFlag;
    DisplaySideType displaySideType;
    BlendType blendType;
    ZBufATestType zBufATestType;
    AnimationType scaleAnimType;
    AnimationType colorAnimType[2];
    AnimationType alphaAnimType[2];
    u32 emitFunction;
    u32 primitiveEmitType;
    math::VEC3 volumeScale;
    f32 arcStartAngle;
    f32 arcLength;
    f32 arcRandom;
    f32 volumeFillRatio;
    f32 sphereLatitude;
    math::VEC3 sphereLatitudeDir;
    f32 lineCenter;
    math::VEC3 emissionShapeScale;
    ut::Color4f emitterColor0;
    ut::Color4f emitterColor1;
    f32 emitterAlpha;
    math::MTX34 animMatrixSRT;
    math::MTX34 animMatrixRT;
    math::VEC3 emitterScale;
    math::VEC3 emitterRotate;
    math::VEC3 emitterTranslate;
    math::VEC3 emitterRotateRandom;
    math::VEC3 emitterTranslateRandom;
    f32 emissionRate;
    s32 emissionRateRandom;
    s32 startFrame;
    s32 endFrame;
    s32 emitInterval;
    s32 emitIntervalRandom;
    f32 allDirVel;
    f32 dirVelRandom;
    f32 dirVel;
    math::VEC3 dir;
    f32 dispersionAngle;
    math::VEC3 diffusionVel;
    f32 airResist;
    f32 yDiffusionVel;
    f32 ptclPosRandom;
    f32 emitterVelInheritRatio;
    f32 emitSameDistanceUnit;
    f32 emitSameDistanceMax;
    f32 emitSameDistanceMin;
    f32 emitSameDistanceThreshold;
    math::VEC3 gravity;
    s32 ptclMaxLifespan;
    s32 ptclLifespanRandom;
    MeshType meshType;
    VertexTransformMode vertexTransformMode;
    f32 momentumRandom;
    math::VEC2 rotBasis;
    u32 cameraOffsetType;
    f32 cameraOffset;
    TextureEmitterData texAnimParam[3];
    ColorSourceType ptclColorSrc[2];
    ut::Color4f ptclColorTbl[2][8];
    s32 colorTime2[2];
    s32 colorTime3[2];
    s32 colorTime4[2];
    s32 colorNumRepetition[2];
    s32 colorRandomStart[2];
    f32 colorScaleFactor;
    AlphaSourceType ptclAlphaSrc[2];
    anim3v4Key alphaAnim[2];
    FragmentComposite texture1ColorBlend;
    FragmentComposite texture2ColorBlend;
    FragmentComposite primitiveColorBlend;
    FragmentComposite texture1AlphaBlend;
    FragmentComposite texture2AlphaBlend;
    FragmentComposite primitiveAlphaBlend;
    s32 scaleAnimTime2;
    s32 scaleAnimTime3;
    math::VEC2 ptclScaleRandom;
    math::VEC2 ptclEmitScale;
    math::VEC2 ptclScaleStart;
    math::VEC2 ptclScaleStartDiff;
    math::VEC2 ptclScaleEndDiff;
    f32 _7F4;
    math::VEC3 ptclRotate;
    math::VEC3 ptclRotateRandom;
    math::VEC3 angularVelocity;
    math::VEC3 angularVelocityRandom;
    f32 rotInertia;
    f32 fadeAlphaStep;
    u8 fragmentShaderMode;
    u8 shaderUserSetting;
    bool fragmentSoftEdge;
    bool enableFresnelAlpha;
    bool enableNearAlpha;
    bool enableFarAlpha;
    bool enableDecal;
    bool refractionApplyAlpha;
    bool enableShaderParamAnim;
    math::VEC2 shaderParam01;
    u32 shaderIdx;
    u32 userShaderIdx1;
    u32 userShaderIdx2;
    u32 _850;
    f32 fragmentSoftEdgeFadeDist;
    f32 fragmentSoftEdgeAlphaOffset;
    f32 fragmentSoftEdgeVolume;
    math::VEC2 fresnelMinMax;
    f32 decalVolume;
    math::VEC2 nearAlphaMinMax;
    math::VEC2 farAlphaMinMax;
    char userMacro1[16];
    char userMacro2[16];
    u32 shaderUserFlag;
    u32 shaderUserSwitchFlag;
    u32 _8A4;
    anim3v4Key shaderParamAnim;
};
static_assert(sizeof(SimpleEmitterData) == 0x8BC, "SimpleEmitterData size mismatch");

struct ComplexEmitterData : SimpleEmitterData
{
    u32 childFlags;
    u16 fieldFlags;
    u16 fluctuationFlags;
    u16 stripeFlags;
    u8 _unusedPad[2];
    u16 childDataOffs;
    u16 fieldDataOffs;
    u16 fluctuationDataOffs;
    u16 stripeDataOffs;
    u32 dataSize;
};
static_assert(sizeof(ComplexEmitterData) == 0x8D4, "ComplexEmitterData size mismatch");

struct ChildData
{
    s32 numChildParticles;
    s32 startFramePercent;
    s32 ptclMaxLifespan;
    s32 emissionInterval;
    u32 particleBehaviorFlg;
    f32 velInheritRatio;
    f32 allDirVel;
    math::VEC3 diffusionVel;
    f32 ptclPosRandom;
    EmitterPrimitive primitive;
    f32 momentumRandom;
    BlendType blendType;
    MeshType meshType;
    VertexTransformMode vertexTransformMode;
    ZBufATestType zBufATestType;
    TextureRes texture;
    DisplaySideType displaySideType;
    math::VEC3 ptclColor0;
    math::VEC3 ptclColor1;
    f32 colorScaleFactor;
    FragmentComposite primitiveColorBlend;
    FragmentComposite primitiveAlphaBlend;
    AlphaSourceType ptclAlphaSrc[2];
    anim3v4Key alphaAnim[2];
    f32 scaleInheritRatio;
    math::VEC2 ptclEmitScale;
    f32 ptclScaleRandom;
    VertexRotationMode rotationMode;
    math::VEC3 ptclRotate;
    math::VEC3 ptclRotateRandom;
    math::VEC3 angularVelocity;
    math::VEC3 angularVelocityRandom;
    f32 rotInertia;
    u8 rotateDirRandomX;
    u8 rotateDirRandomY;
    u8 rotateDirRandomZ;
    u8 primitiveScaleYToZ;
    math::VEC2 rotBasis;
    math::VEC3 gravity;
    s32 scaleAnimTime1;
    math::VEC2 ptclScaleEnd;
    TextureEmitterData texAnimParam;
    u32 fragmentColorMode;
    u32 fragmentAlphaMode;
    f32 airResist;
    u8 fragmentShaderMode;
    u8 shaderUserSetting;
    bool fragmentSoftEdge;
    bool enableFresnelAlpha;
    bool enableNearAlpha;
    bool enableFarAlpha;
    bool refractionApplyAlpha;
    bool enableDecal;
    bool enableShaderParamAnim;
    math::VEC2 shaderParam01;
    u32 shaderIdx;
    u32 userShaderIdx1;
    u32 userShaderIdx2;
    u32 _290;
    f32 fragmentSoftEdgeFadeDist;
    f32 fragmentSoftEdgeAlphaOffset;
    f32 fragmentSoftEdgeVolume;
    math::VEC2 fresnelMinMax;
    math::VEC2 nearAlphaMinMax;
    math::VEC2 farAlphaMinMax;
    f32 decalVolume;
    char userMacro1[16];
    char userMacro2[16];
    u32 shaderUserFlag;
    u32 shaderUserSwitchFlag;
    EmitterShaderParam shaderParam;
    u32 _2F0;
    anim3v4Key shaderParamAnim;
};
static_assert(sizeof(ChildData) == 0x308, "ChildData size mismatch");

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
    u8 followEmitter;
    // u8 pad[3];
};
static_assert(sizeof(FieldMagnetData) == 0x18, "FieldMagnetData size mismatch");

struct FieldSpinData
{
    u32 angle;
    s32 axis;
    f32 diffusionVel;
};
static_assert(sizeof(FieldSpinData) == 0xC, "FieldSpinData size mismatch");

struct FieldCollisionData
{
    u8 collisionType;
    u8 coordSystem;
    u8 sharedPlane;
    f32 y;
    f32 bounceRate;
    s32 bounceCount;
    f32 friction;
};
static_assert(sizeof(FieldCollisionData) == 0x14, "FieldCollisionData size mismatch");

struct FieldConvergenceData
{
    u32 followType;
    math::VEC3 pos;
    f32 strength;
};
static_assert(sizeof(FieldConvergenceData) == 0x14, "FieldConvergenceData size mismatch");

struct FieldPosAddData
{
    math::VEC3 posAdd;
    u8 coordSystem;
    // u8 pad[3];
};
static_assert(sizeof(FieldPosAddData) == 0x10, "FieldPosAddData size mismatch");

struct FieldCurlNoiseData
{
    u8 interpolation;
    u8 randomOffset;
    f32 scale;
    math::VEC3 weight;
    math::VEC3 speed;
    f32 offset;
};
static_assert(sizeof(FieldCurlNoiseData) == 0x24, "FieldCurlNoiseData size mismatch");

struct FluctuationData
{
    struct
    {
        f32 amplitude;
        f32 frequency;
        u32 enableRandom;
        f32 phase;
    } x, y;
};
static_assert(sizeof(FluctuationData) == 0x20, "FluctuationData size mismatch");

struct StripeData
{
    u32 type;
    u32 crossType;
    u32 connectionType;
    u32 textureType;
    u32 numSliceHistory;
    u32 numDivisions;
    f32 alphaStart;
    f32 alphaEnd;
    u8 _unusedPad[8];
    u32 sliceHistInterval;
    f32 sliceInterpolation;
    f32 dirInterpolation;
};
static_assert(sizeof(StripeData) == 0x34, "StripeData size mismatch");

struct EmitterStaticUniformBlock;

struct EmitterTblData
{
    u32 dataOffs;
    EmitterData* data;
    EmitterStaticUniformBlock* emitterStaticUniformBlock;
    EmitterStaticUniformBlock* childEmitterStaticUniformBlock;
};
static_assert(sizeof(EmitterTblData) == 0x10, "EmitterTblData size mismatch");

struct EmitterSetData // Actual name not known
{
    u32 userData;
    u8 _unusedPad[4];
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
    u8 _unusedPad0[4];
    u32 strTblOffs;
    u32 textureDataTblOffs;
    u32 textureDataTblSize;
    u32 shaderTblOffs;
    u32 shaderTblSize;
    u32 keyAnimArrayTblOffs;
    u32 keyAnimArrayTblSize;
    u32 primitiveTblOffs;
    u32 primitiveTblSize;
    u32 shaderParamTblOffs;
    u32 shaderParamTblSize;
    u8 _unusedPad1[12];
};
static_assert(sizeof(Header) == 0x48, "Header size mismatch");

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

struct KeyFrameAnim
{
    u32 numKeys;
    u32 interpolation;
    u32 animValIdx;
    u32 loop;
    u32 loopLength;
    u32 randomStart;
    u32 nextOffs;
};
static_assert(sizeof(KeyFrameAnim) == 0x1C, "KeyFrameAnim size mismatch");

struct KeyFrameAnimKey
{
    f32 time;
    union
    {
        f32 value;
        s32 valueS32;
    };
};
static_assert(sizeof(KeyFrameAnimKey) == 8, "KeyFrameAnimKey size mismatch");

} } // namespace nw::eft

#endif // EFT_RES_DATA_H_
