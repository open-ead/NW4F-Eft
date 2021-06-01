#ifndef EFT_EMITTER_H_
#define EFT_EMITTER_H_

#include <math/math_MTX34.h>
#include <math/math_VEC3.h>
#include <eft_Random.h>
#include <eft_Shader.h>

namespace nw { namespace eft {

class EmitterCalc;
class EmitterController;
struct EmitterDynamicUniformBlock;
class EmitterSet;
class KeyFrameAnimArray;
class Primitive;
struct PtclAttributeBuffer;
struct PtclInstance;
class StripeVertexBuffer;

struct EmitterInstance
{
    void Init(const SimpleEmitterData* data);
    void UpdateResInfo();

    void UpdateEmitterStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const SimpleEmitterData* data)
    {
        for (u32 i = 0; i < ShaderType_Max; i++)
        {
            ParticleShader* shader = this->shader[i];
            if (shader == NULL)
                continue;

            if (shader->attrSclBuffer != 0xFFFFFFFF)
                shaderAvailableAttribFlg |= 0x001;

            if (shader->attrSubTexAnimBuffer != 0xFFFFFFFF) // Not attrTexAnimBuffer. Typo?
                shaderAvailableAttribFlg |= 0x002;

            if (shader->attrSubTexAnimBuffer != 0xFFFFFFFF)
                shaderAvailableAttribFlg |= 0x004;

            if (shader->attrWldPosBuffer != 0xFFFFFFFF)
                shaderAvailableAttribFlg |= 0x008;

            if (shader->attrWldPosDfBuffer != 0xFFFFFFFF)
                shaderAvailableAttribFlg |= 0x010;

            if (shader->attrColor0Buffer != 0xFFFFFFFF)
                shaderAvailableAttribFlg |= 0x020;

            if (shader->attrColor1Buffer != 0xFFFFFFFF)
                shaderAvailableAttribFlg |= 0x040;

            if (shader->attrRotBuffer != 0xFFFFFFFF)
                shaderAvailableAttribFlg |= 0x080;

            if (shader->attrEmMat0Buffer != 0xFFFFFFFF)
                shaderAvailableAttribFlg |= 0x100;
        }

        for (u32 i = 0; i < ShaderType_Max; i++)
        {
            ParticleShader* shader = this->childShader[i];
            if (shader == NULL)
                continue;

            if (shader->attrSclBuffer != 0xFFFFFFFF)
                childShaderAvailableAttribFlg |= 0x001;

            if (shader->attrSubTexAnimBuffer != 0xFFFFFFFF) // Not attrTexAnimBuffer. Typo?
                childShaderAvailableAttribFlg |= 0x002;

            if (shader->attrSubTexAnimBuffer != 0xFFFFFFFF)
                childShaderAvailableAttribFlg |= 0x004;

            if (shader->attrWldPosBuffer != 0xFFFFFFFF)
                childShaderAvailableAttribFlg |= 0x008;

            if (shader->attrWldPosDfBuffer != 0xFFFFFFFF)
                childShaderAvailableAttribFlg |= 0x010;

            if (shader->attrColor0Buffer != 0xFFFFFFFF)
                childShaderAvailableAttribFlg |= 0x020;

            if (shader->attrColor1Buffer != 0xFFFFFFFF)
                childShaderAvailableAttribFlg |= 0x040;

            if (shader->attrRotBuffer != 0xFFFFFFFF)
                childShaderAvailableAttribFlg |= 0x080;

            if (shader->attrEmMat0Buffer != 0xFFFFFFFF)
                childShaderAvailableAttribFlg |= 0x100;
        }

        if (data->airResist != 1.0f)
            particleBehaviorFlg |= 0x0001;

        if (data->gravity.Magnitude() != 0.0f)
            particleBehaviorFlg |= 0x0002;

        if (data->rotationMode != 0)
            particleBehaviorFlg |= 0x0004;

        if (data->rotInertia != 1.0f)
            particleBehaviorFlg |= 0x0008;

        if (shaderAvailableAttribFlg & 0x10)
            particleBehaviorFlg |= 0x0010;

        if (data->scaleAnimTime2 != -127 || data->scaleAnimTime3 != 100)
            particleBehaviorFlg |= 0x0040;

        if (data->alphaAnim.time2 != 0 || data->alphaAnim.time3 != 100)
            particleBehaviorFlg |= 0x0080;

        if (data->ptclColor0Src == ColorSourceType_3v4k)
            particleBehaviorFlg |= 0x0100;

        if (data->ptclColor1Src == ColorSourceType_3v4k)
            particleBehaviorFlg |= 0x0200;

        if (data->texAnimParam[0].uvShiftAnimMode != 0)
            particleBehaviorFlg |= 0x0400;

        if (data->texAnimParam[1].uvShiftAnimMode != 0)
            particleBehaviorFlg |= 0x0800;

        if (data->texAnimParam[0].hasTexPtnAnim)
            particleBehaviorFlg |= 0x1000;

        if (data->texAnimParam[1].hasTexPtnAnim)
            particleBehaviorFlg |= 0x2000;

        if (data->textures[1].initialized != 0)
            particleBehaviorFlg |= 0x4000;

        uniformBlock->uvScaleInit.xy() = data->texAnimParam[0].uvScaleInit;
        uniformBlock->uvScaleInit.zw() = data->texAnimParam[1].uvScaleInit;

        uniformBlock->rotBasis.xy() = data->rotBasis;
        uniformBlock->rotBasis.z = 0.0f;
        uniformBlock->rotBasis.w = 0.0f;

        uniformBlock->shaderParam.xy() = data->shaderParam01;
        uniformBlock->shaderParam.z = data->fragmentSoftEdgeFadeDist;
        uniformBlock->shaderParam.w = data->fragmentSoftEdgeVolume;

        GX2EndianSwap(uniformBlock, sizeof(EmitterStaticUniformBlock));
        DCFlushRange(uniformBlock, sizeof(EmitterStaticUniformBlock));
    }

    void UpdateChildStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const ChildData* data)
    {
        uniformBlock->uvScaleInit.xy() = data->uvScaleInit;
        uniformBlock->uvScaleInit.z = 0.0f;
        uniformBlock->uvScaleInit.w = 0.0f;

        uniformBlock->rotBasis.xy() = data->rotBasis;
        uniformBlock->rotBasis.z = 0.0f;
        uniformBlock->rotBasis.w = 0.0f;

        uniformBlock->shaderParam.xy() = data->shaderParam01;
        uniformBlock->shaderParam.z = data->fragmentSoftEdgeFadeDist;
        uniformBlock->shaderParam.w = data->fragmentSoftEdgeVolume;

        GX2EndianSwap(uniformBlock, sizeof(EmitterStaticUniformBlock));
        DCFlushRange(uniformBlock, sizeof(EmitterStaticUniformBlock));
    }

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

    static void InitializeFluctuationTable(Heap* heap);

    static f32* sFluctuationTbl;
    static System* mSys;
};
static_assert(sizeof(EmitterCalc) == 4, "EmitterCalc size mismatch");

} } // namespace nw::eft

#endif // EFT_EMITTER_H_
