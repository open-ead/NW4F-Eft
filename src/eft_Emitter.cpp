#include <eft_Emitter.h>
#include <eft_Shader.h>
#include <eft_System.h>

namespace nw { namespace eft {

System* EmitterCalc::mSys = NULL;

void EmitterInstance::Init(const SimpleEmitterData* data)
{
    this->data = data;

    counter = 0.0f;
    counter2 = 0.0f;
    emitCounter = 0.0f;
    preCalcCounter = 0.0f;
    emitLostTime = 0.0f;
    numParticles = 0;
    numChildParticles = 0;

    emissionInterval = data->emitInterval - random.GetS32(data->emitIntervalRandom);
    fadeAlpha = 1.0f;
    emissionSpeed = 1.0f;

    ptclFollowType = data->ptclFollowType;

    particleHead = NULL;
    childParticleHead = NULL;
    particleTail = NULL;
    childParticleTail = NULL;

    scaleRandom.x = 0.0f;
    scaleRandom.y = 0.0f;
    scaleRandom.z = 0.0f;

    rotateRandom.x = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.x;
    rotateRandom.y = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.y;
    rotateRandom.z = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.z;

    translateRandom.x = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.x;
    translateRandom.y = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.y;
    translateRandom.z = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.z;

    _1EA = 0;

    animArray = static_cast<KeyFrameAnimArray*>(data->keyAnimArray);

    emitLostRate = 0.0f;
    isEmitted = false;
    isCalculated = false;

    for (u32 i = 0; i < ShaderType_Max; i++)
    {
        shader[i] = NULL;
        childShader[i] = NULL;
    }

    primitive = NULL;
    childPrimitive = NULL;

    math::MTX34::Copy(&animMatrixRT,  &math::MTX34::Identity());
    math::MTX34::Copy(&animMatrixSRT, &math::MTX34::Identity());

    prevPosSet = false;
    emitLostDistance = 0.0f;
}

void EmitterInstance::UpdateEmitterStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const SimpleEmitterData* data)
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

    if (data->rotationMode != VertexRotationMode_None)
        particleBehaviorFlg |= 0x0004;

    if (data->rotInertia != 1.0f)
        particleBehaviorFlg |= 0x0008;

    if (shaderAvailableAttribFlg & 0x10)
        particleBehaviorFlg |= 0x0010;

    if (data->scaleAnimTime2 != -127 || data->scaleAnimTime3 != 100)
        particleBehaviorFlg |= 0x0040;

    if (data->alphaAnim.time2 != 0 || data->alphaAnim.time3 != 100)
        particleBehaviorFlg |= 0x0080;

    if (data->ptclColorSrc[0] == ColorSourceType_3v4k)
        particleBehaviorFlg |= 0x0100;

    if (data->ptclColorSrc[1] == ColorSourceType_3v4k)
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

void EmitterInstance::UpdateChildStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const ChildData* data)
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

void EmitterInstance::UpdateResInfo()
{
    ptclFollowType = data->ptclFollowType;

    anim[ 0] = data->emissionRate;
    anim[ 1] = data->ptclMaxLifespan;
    anim[15] = data->ptclVelocityMag;
    anim[16] = data->emitterVelocityMag;
    anim[14] = data->emitterAlpha;
    anim[11] = data->emitterColor0.r;
    anim[12] = data->emitterColor0.g;
    anim[13] = data->emitterColor0.b;
    anim[19] = data->emitterColor1.r;
    anim[20] = data->emitterColor1.g;
    anim[21] = data->emitterColor1.b;
    anim[22] = data->emissionShapeScale.x;
    anim[23] = data->emissionShapeScale.y;
    anim[24] = data->emissionShapeScale.z;
    anim[17] = 1.0f;
    anim[18] = 1.0f;
    /* WTF Nintendo
    anim[ 2] = data->emitterScale.x;
    anim[ 3] = data->emitterScale.y;
    anim[ 4] = data->emitterScale.z;
    anim[ 5] = data->emitterRotate.x;
    anim[ 6] = data->emitterRotate.y;
    anim[ 7] = data->emitterRotate.z;
    anim[ 8] = data->emitterTranslate.x;
    anim[ 9] = data->emitterTranslate.y;
    anim[10] = data->emitterTranslate.z;
    */
    anim[ 2] = data->emitterScale.x + scaleRandom.x;
    anim[ 3] = data->emitterScale.y + scaleRandom.y;
    anim[ 4] = data->emitterScale.z + scaleRandom.z;
    anim[ 5] = data->emitterRotate.x + rotateRandom.x;
    anim[ 6] = data->emitterRotate.y + rotateRandom.y;
    anim[ 7] = data->emitterRotate.z + rotateRandom.z;
    anim[ 8] = data->emitterTranslate.x + translateRandom.x;
    anim[ 9] = data->emitterTranslate.y + translateRandom.x;
    anim[10] = data->emitterTranslate.z + translateRandom.x;

    particleBehaviorFlg = 0;
    shaderAvailableAttribFlg = 0;
    childShaderAvailableAttribFlg = 0;

    UpdateEmitterStaticUniformBlock(emitterStaticUniformBlock, data);

    if (HasChild())
        UpdateChildStaticUniformBlock(childEmitterStaticUniformBlock, GetChildData());
}

void EmitterCalc::RemoveParticle(EmitterInstance* emitter, PtclInstance* ptcl, CpuCore core)
{
    mSys->AddPtclRemoveList(ptcl, core);
}

} } // namespace nw::eft
