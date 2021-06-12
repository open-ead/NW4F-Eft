#ifndef EFT_EMITTER_H_
#define EFT_EMITTER_H_

#include <eft_Particle.h>
#include <eft_Random.h>
#include <eft_ResData.h>

namespace nw { namespace eft {

class EmitterCalc;
class EmitterController;
struct EmitterDynamicUniformBlock;
class EmitterSet;
struct EmitterStaticUniformBlock;
class KeyFrameAnimArray;
class ParticleShader;
class Primitive;
struct PtclAttributeBuffer;
class StripeVertexBuffer;

struct EmitterInstance
{
    void Init(const SimpleEmitterData* data);
    inline void UpdateEmitterStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const SimpleEmitterData* data);
    inline void UpdateChildStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const ChildData* data);
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

    f32 counter;
    f32 counter2;
    f32 emitCounter;
    f32 preCalcCounter;
    f32 emitLostTime;
    u32 numParticles;
    u32 numChildParticles;
    u32 groupID;
    EmitterSet* emitterSet;
    EmitterController* controller;
    u32 emitterSetCreateID;
    math::MTX34 matrixRT;
    math::MTX34 matrixSRT;
    PtclRandom random;
    s32 emissionInterval;
    f32 fadeAlpha;
    f32 emissionSpeed;
    math::VEC3 prevPos; // Used for equidistant emission
    bool prevPosSet;
    f32 emitLostDistance;
    math::VEC3 scaleRandom;
    math::VEC3 rotateRandom;
    math::VEC3 translateRandom;
    PtclFollowType ptclFollowType;
    EmitterInstance* prev;
    EmitterInstance* next;
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
    KeyFrameAnimArray* animArray;
    f32 anim[25];
    math::MTX34 animMatrixRT;
    math::MTX34 animMatrixSRT;
    f32 emitLostRate;
    bool isEmitted;
    bool isCalculated;
    u8 _1EA;
    u32 particleBehaviorFlg;
    u32 shaderAvailableAttribFlg;
    u32 childShaderAvailableAttribFlg;
    u32 numDrawParticle;
    u32 numDrawChildParticle;
    u32 numDrawStripe;
    PtclAttributeBuffer* ptclAttributeBuffer;
    PtclAttributeBuffer* childPtclAttributeBuffer;
    StripeVertexBuffer* stripeVertexBuffer;
    EmitterStaticUniformBlock* emitterStaticUniformBlock;
    EmitterStaticUniformBlock* childEmitterStaticUniformBlock;
    EmitterDynamicUniformBlock* emitterDynamicUniformBlock;
    EmitterDynamicUniformBlock* childEmitterDynamicUniformBlock;
};
static_assert(sizeof(EmitterInstance) == 0x220, "EmitterInstance size mismatch");

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
    virtual EmitterType GetEmitterType() const = 0;
    virtual u32 CalcParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer) = 0;
    virtual u32 CalcChildParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer) = 0;

    static void RemoveParticle(EmitterInstance* emitter, PtclInstance* ptcl, CpuCore core);
    static inline void AddChildPtclToList(EmitterInstance* emitter, PtclInstance* childPtcl);
    static inline void AddPtclToList(EmitterInstance* emitter, PtclInstance* ptcl);
    static void InitializeFluctuationTable(Heap* heap);

    static f32* sFluctuationTbl;
    static System* mSys;

    typedef void (*EmitFunction)(EmitterInstance* emitter);
    static EmitFunction mEmitFunctions[];
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

} } // namespace nw::eft

#endif // EFT_EMITTER_H_
