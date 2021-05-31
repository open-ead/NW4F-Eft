#ifndef EFT_EMITTER_H_
#define EFT_EMITTER_H_

#include <math/math_MTX34.h>
#include <math/math_VEC3.h>
#include <eft_Enum.h>
#include <eft_Random.h>

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
struct PtclInstance;
struct SimpleEmitterData;
class StripeVertexBuffer;

struct EmitterInstance
{
    f32 counter;
    f32 counter2;
    f32 emitCounter;
    f32 emitCounter2;
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
    SimpleEmitterData* data;
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
    virtual u32 CalcParticle(EmitterInstance* emitter, CpuCore core, bool, bool) = 0;
    virtual u32 CalcChildParticle(EmitterInstance* emitter, CpuCore core, bool, bool) = 0;

    static void InitializeFluctuationTable(Heap* heap);

    static f32* sFluctuationTbl;
    static System* mSys;
};
static_assert(sizeof(EmitterCalc) == 4, "EmitterCalc size mismatch");

} } // namespace nw::eft

#endif // EFT_EMITTER_H_
