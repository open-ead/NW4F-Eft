#ifndef EFT_EMITTER_H_
#define EFT_EMITTER_H_

#include <math/math_MTX34.h>
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
    f32 prevPos[3]; // VEC3, used for equidistant emission
    bool prevPosSet;
    f32 emitLostDistance;
    f32 scaleRandom[3]; // VEC3
    f32 rotateRandom[3]; // VEC3
    f32 translateRandom[3]; // VEC3
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

} } // namespace nw::eft

#endif // EFT_EMITTER_H_
