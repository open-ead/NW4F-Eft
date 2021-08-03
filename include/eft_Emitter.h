#ifndef EFT_EMITTER_H_
#define EFT_EMITTER_H_

#include <eft_Particle.h>
#include <eft_Random.h>
#include <eft_ResData.h>
#include <eft_StreamOutBuffer.h>

namespace nw { namespace eft {

class EmitterCalc;
class EmitterController;
struct EmitterDynamicUniformBlock;
class EmitterSet;
struct EmitterStaticUniformBlock;
struct KeyFrameAnimArray;
class ParticleShader;
class Primitive;
struct PtclAttributeBuffer;
struct PtclAttributeBufferGpu;
struct StripeUniformBlock;
class StripeVertexBuffer;

struct EmitterInstance
{
    void Init(const SimpleEmitterData* data);
    static void UpdateEmitterStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const SimpleEmitterData* data, const ComplexEmitterData* cdata);
    static void UpdateChildStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const ChildData* data);
    void UpdateEmitterInfoByEmit();
    void UpdateResInfo();

    const ComplexEmitterData* GetComplexEmitterData() const
    {
        if (data->type != EmitterType_Complex)
            return NULL;

        return static_cast<const ComplexEmitterData*>(data);
    }

    bool HasChild() const
    {
        return data->type != EmitterType_Simple && (static_cast<const ComplexEmitterData*>(data)->childFlags & 1);
    }

    const ChildData* GetChildData() const
    {
        if (!HasChild())
            return NULL;

        return reinterpret_cast<const ChildData*>(static_cast<const ComplexEmitterData*>(data) + 1);
    }

    bool IsStripe() const
    {
        return data->type != EmitterType_Simple && (data->vertexTransformMode == VertexTransformMode_Stripe || data->vertexTransformMode == VertexTransformMode_Complex_Stripe);
    }

    const StripeData* GetStripeData() const
    {
        if (!IsStripe())
            return NULL;

        const ComplexEmitterData* cdata = static_cast<const ComplexEmitterData*>(data);
        return reinterpret_cast<const StripeData*>((u32)cdata + cdata->stripeDataOffs);
    }

    u8 _unusedPad0[16];
    u32 numParticles;
    u32 numChildParticles;
    u32 groupID;
    s32 emissionInterval;
    u32 emitterBehaviorFlg;
    u32 particleBehaviorFlg;
    u32 childParticleBehaviorFlg;
    f32 emitLostDistance;
    f32 counter;
    f32 gpuCounter;
    f32 counter2;
    f32 emitCounter;
    f32 preCalcCounter;
    f32 emitLostTime;
    f32 fieldCollisionY;
    u32 primitiveEmitCounter;
    f32 fadeStartFrame;
    f32 randomF32;
    f32 fadeAlpha;
    f32 emissionSpeed;
    f32 emitLostRate;
    u32 numDrawParticle;
    u32 currentPtclAttributeBufferGpuIdx;
    u32 numPtclAttributeBufferGpuMax;
    u32 numDrawChildParticle;
    u32 numDrawStripe;
    u32 emitterSetCreateID;
    EmitterSet* emitterSet;
    EmitterController* controller;
    math::MTX34 matrixRT;
    math::MTX34 matrixSRT;
    PtclRandom random;
    math::VEC3 prevPos;
    math::VEC3 scaleRandom;
    math::VEC3 rotateRandom;
    math::VEC3 translateRandom;
    PtclFollowType ptclFollowType;
    EmitterInstance* prev;
    EmitterInstance* next;
    EmitterInstance* nextStreamOut;
    EmitterCalc* calc;
    const SimpleEmitterData* data;
    PtclInstance* particleHead;
    PtclInstance* childParticleHead;
    PtclInstance* particleTail;
    PtclInstance* childParticleTail;
    ParticleShader* shader[ShaderType_Max];
    ParticleShader* childShader[ShaderType_Max];
    Primitive* primitive;
    Primitive* childPrimitive;
    Primitive* volumePrimitive;
    KeyFrameAnimArray* animArray;
    u32 numUsedAnim;
    KeyFrameAnim* usedAnimArray[27];
    f32 anim[27];
    KeyFrameAnim* ptclAnimArray[20];
    math::MTX34 animMatrixRT;
    math::MTX34 animMatrixSRT;
    PtclAttributeBuffer* ptclAttributeBuffer;
    PtclAttributeBuffer* childPtclAttributeBuffer;
    PtclAttributeBufferGpu* ptclAttributeBufferGpu;
    StripeVertexBuffer* stripeVertexBuffer;
    EmitterStaticUniformBlock* emitterStaticUniformBlock;
    EmitterStaticUniformBlock* childEmitterStaticUniformBlock;
    EmitterDynamicUniformBlock* emitterDynamicUniformBlock;
    EmitterDynamicUniformBlock* childEmitterDynamicUniformBlock;
    StripeUniformBlock* connectionStripeUniformBlock;
    StripeUniformBlock* connectionStripeUniformBlockCross;
    StreamOutAttributeBuffer posStreamOutAttributeBuffer;
    StreamOutAttributeBuffer vecStreamOutAttributeBuffer;
    bool swapStreamOut;
    u32 _unused;
    u32 userData;
    u8 _unusedPad1[4];
};
static_assert(sizeof(EmitterInstance) == 0x3E0, "EmitterInstance size mismatch");

class System;

class EmitterCalc
{
public:
    EmitterCalc(System* system)
    {
        mSys = system;
    }

    virtual ~EmitterCalc() { } // deleted

    virtual void CalcEmitter(EmitterInstance* emitter) = 0;
    virtual PtclType GetPtclType() const = 0;
    virtual u32 CalcParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer) = 0;
    virtual u32 CalcChildParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer) = 0;
    virtual EmitterDynamicUniformBlock* MakeEmitterUniformBlock(EmitterInstance* emitter, CpuCore core, const ChildData* childData, bool noCalcBehavior) = 0;

    static void RemoveParticle(PtclInstance* ptcl, CpuCore core);
    static inline void AddChildPtclToList(EmitterInstance* emitter, PtclInstance* childPtcl);
    static void AddPtclToList(EmitterInstance* emitter, PtclInstance* ptcl);
    static void EmitCommon(EmitterInstance* emitter, PtclInstance* ptcl);

    static const void* _ptclField_Random(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed);
    static const void* _ptclField_Magnet(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed);
    static const void* _ptclField_Spin(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed);
    static const void* _ptclField_Collision(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed);
    static const void* _ptclField_Convergence(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed);
    static const void* _ptclField_PosAdd(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed);
    static const void* _ptclField_CurlNoise(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed);

    static void InitializeFluctuationTable();
    static void CalcFluctuation(EmitterInstance* emitter, PtclInstance* ptcl);

    static void CalcSimpleParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void CalcComplexParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void CalcChildParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void MakeParticleAttributeBuffer(PtclAttributeBuffer* ptclAttributeBuffer, PtclInstance* ptcl, u32 shaderAvailableAttribFlg);
    static void ptclAnim_Scale_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Scale_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Color0_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Color1_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Color0_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Color1_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Alpha0_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Alpha0_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Alpha1_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void ptclAnim_Alpha1_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static void CalcPtclAnimation(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed) { };

    void ApplyAnim(EmitterInstance* emitter);

    static void CalcEmitPoint(EmitterInstance* emitter);
    static void CalcEmitCircle(EmitterInstance* emitter);
    static void CalcEmitCircleSameDivide(EmitterInstance* emitter);
    static void CalcEmitFillCircle(EmitterInstance* emitter);
    static void CalcEmitSphere(EmitterInstance* emitter);
    static void CalcEmitSphereSameDivide(EmitterInstance* emitter);
    static void CalcEmitSphereSameDivide64(EmitterInstance* emitter);
    static void CalcEmitFillSphere(EmitterInstance* emitter);
    static void CalcEmitCylinder(EmitterInstance* emitter);
    static void CalcEmitFillCylinder(EmitterInstance* emitter);
    static void CalcEmitBox(EmitterInstance* emitter);
    static void CalcEmitFillBox(EmitterInstance* emitter);
    static void CalcEmitLine(EmitterInstance* emitter);
    static void CalcEmitLineSameDivide(EmitterInstance* emitter);
    static void CalcEmitRectangle(EmitterInstance* emitter);
    static void CalcEmitPrimitive(EmitterInstance* emitter);

    static f32* sFluctuationTbl;
    static f32* sFluctuationSawToothTbl;
    static f32* sFluctuationRectTbl;
    static System* mSys;

    typedef void (*EmitFunction)(EmitterInstance* emitter);
    static EmitFunction mEmitFunctions[];

    typedef void (*AnimFunction)(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed);
    static AnimFunction mAnimFunctionsSclae[AnimationType_Max];
    static AnimFunction mAnimFunctionsColor0[AnimationType_Max];
    static AnimFunction mAnimFunctionsColor1[AnimationType_Max];
    static AnimFunction mAnimFunctionsAlpha0[AnimationType_Max];
    static AnimFunction mAnimFunctionsAlpha1[AnimationType_Max];
};
static_assert(sizeof(EmitterCalc) == 4, "EmitterCalc size mismatch");

void EmitterCalc::AddChildPtclToList(EmitterInstance* emitter, PtclInstance* childPtcl)
{
    if (emitter->childParticleHead == NULL)
    {
        emitter->childParticleHead = childPtcl;
        childPtcl->next = NULL;
        childPtcl->prev = NULL;
    }
    else
    {
        emitter->childParticleHead->prev = childPtcl;
        childPtcl->next = emitter->childParticleHead;
        emitter->childParticleHead = childPtcl;
        childPtcl->prev = NULL;
    }

    if (emitter->childParticleTail == NULL)
        emitter->childParticleTail = childPtcl;

    emitter->numChildParticles++;
}

f32 _initialize3v4kAnim(AlphaAnim* anim, const anim3v4Key* key, s32 lifespan);
u32 _getUnifiedAnimID(u32 animValIdx);
f32 _calcParticleAnimTime(EmitterInstance* emitter, PtclInstance* ptcl, u32 animValIdx);
f32 _calcParticleAnimTime(KeyFrameAnim* anim, PtclInstance* ptcl, u32 animValIdx);

} } // namespace nw::eft

#endif // EFT_EMITTER_H_
