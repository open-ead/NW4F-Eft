#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
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

void EmitterCalc::EmitCommon(EmitterInstance* emitter, PtclInstance* ptcl)
{
    const SimpleEmitterData* data = emitter->data;
    const EmitterSet* emitterSet = emitter->emitterSet;

    f32 velocityMagRandom = 1.0f - emitter->random.GetF32() * data->_3D8 * emitterSet->_24C;
    f32 velocityMag = emitter->anim[16] * emitterSet->_248;

    if (data->_40C != 0.0f)
        ptcl->pos = emitter->random.GetNormalizedVec3() * data->_40C + ptcl->pos;

    if (emitterSet->_289 != 0)
        ptcl->velocity = (ptcl->velocity + emitterSet->_25C * velocityMag) * velocityMagRandom;

    else
    {
        f32 dispersionAngle = data->_3E8;
        if (dispersionAngle == 0.0f)
            ptcl->velocity = (ptcl->velocity + data->_3DC * velocityMag) * velocityMagRandom;

        else
        {
            dispersionAngle = 1.0f - dispersionAngle / 90.0f;

            f32 sin_val, cos_val, angle = emitter->random.GetF32() * 2.0f * math::F_PI;
            math::SinCosRad(&sin_val, &cos_val, angle);

            f32 y = emitter->random.GetF32() * (1.0f - dispersionAngle) + dispersionAngle;

            f32 a = 1.0f - y * y;
            if (a <= 0.0f)
                a = 0.0f;
            else
                a = sqrtf(a);

            math::VEC3 normalizedVel = (math::VEC3){ a * cos_val, y, a * sin_val };

            math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
            math::MTX34 mtx;
            math::MTX34::MakeVectorRotation(&mtx, &base, &data->_3DC);

            math::MTX34::PSMultVec(&normalizedVel, &mtx, &normalizedVel);
            ptcl->velocity = (ptcl->velocity + normalizedVel * velocityMag) * velocityMagRandom;
        }
    }

    math::VEC3 randomVec3 = emitter->random.GetVec3();
    ptcl->velocity.x += randomVec3.x * data->_3EC.x;
    ptcl->velocity.y += randomVec3.y * data->_3EC.y;
    ptcl->velocity.z += randomVec3.z * data->_3EC.z;

    math::VEC3 addVelocity;
    math::VEC3::MultMTX(&addVelocity, &emitterSet->_250, &emitterSet->matrixRT);
    ptcl->velocity += addVelocity;

    ptcl->posDiff = ptcl->velocity;
    ptcl->counter = 0.0f;
    ptcl->randomU32 = emitter->random.GetU32();

    // ...
}

} } // namespace nw::eft
