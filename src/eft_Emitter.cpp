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

static inline f32 _initialize3v4kAnim(AlphaAnim* anim, const anim3v4Key* key, s32 lifespan)
{
    anim->time2 = (key->time2 * lifespan) / 100;
    anim->time3 = (key->time3 * lifespan) / 100;

    if (anim->time2 == 0)
        anim->startDiff = 0.0f;
    else
        anim->startDiff = key->startDiff / (f32)anim->time2;

    if (key->time3 == 100)
        anim->endDiff = 0.0f;
    else
        anim->endDiff = key->endDiff / (f32)(lifespan - anim->time3);

    return key->startValue - anim->startDiff;
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

    if (data->ptclMaxLifespan == 0x7FFFFFFF)
        ptcl->lifespan = 0x7FFFFFFF;

    else
        ptcl->lifespan = (s32)((emitter->anim[1] - emitter->random.GetS32(data->ptclLifespanRandom)) * emitter->controller->_8);

    s32 lifespan = ptcl->lifespan - 1;
    math::VEC2 scaleRandom = (1.0f - data->_5DC * emitter->random.GetF32()) * emitterSet->_218;

    if (lifespan == 0)
    {
        ptcl->alphaAnim->time2 = -1;
        ptcl->alphaAnim->time3 = 0x7FFFFFFF;
        ptcl->alphaAnim->startDiff = 0.0f;
        ptcl->alphaAnim->endDiff = 0.0f;
        ptcl->alpha = data->alphaAnim.startValue;

        ptcl->scaleAnim->time2 = -1;
        ptcl->scaleAnim->time3 = 0x7FFFFFFF;
        ptcl->scaleAnim->startDiff = math::VEC2::Zero();
        ptcl->scaleAnim->endDiff = math::VEC2::Zero();

        if (emitter->animArray != NULL)
        {
            bool found = false;

            KeyFrameAnim* anim = reinterpret_cast<KeyFrameAnim*>(emitter->animArray + 1);
            if (anim != NULL) // ???
            {
                for (u32 i = 0; i < emitter->animArray->numAnim; i++)
                {
                    if (anim->animValIdx == 17)
                    {
                        found = true;
                        break;
                    }

                    anim = reinterpret_cast<KeyFrameAnim*>((u32)anim + anim->nextOffs);
                }
            }

            if (found)
            {
                ptcl->scale.x = data->_5E8.x * scaleRandom.x * emitter->anim[17];
                ptcl->scale.y = data->_5E8.y * scaleRandom.y * emitter->anim[18];
            }
            else
            {
                ptcl->scale.x = data->_5E8.x * scaleRandom.x * data->_5E0.x;
                ptcl->scale.y = data->_5E8.y * scaleRandom.y * data->_5E0.y;
            }
        }
    }
    else
    {
        ptcl->alpha = _initialize3v4kAnim(ptcl->alphaAnim, &data->alphaAnim, lifespan);

        ptcl->scaleAnim->time2 = 0; // ???
        ptcl->scaleAnim->time3 = 0; // ^^^

        ptcl->scaleAnim->time2 = (data->scaleAnimTime2 * lifespan) / 100;
        ptcl->scaleAnim->time3 = (data->scaleAnimTime3 * lifespan) / 100;

        math::VEC2 scaleAnimStartDiff = data->_5F0 * (1.0f / (f32)ptcl->scaleAnim->time2);
        math::VEC2 scale = data->_5E8 - scaleAnimStartDiff;
        math::VEC2 scaleAnimEndDiff = data->_5F8 * (1.0f / (f32)(lifespan - ptcl->scaleAnim->time3));

        if (emitter->animArray != NULL)
        {
            bool found = false;

            KeyFrameAnim* anim = reinterpret_cast<KeyFrameAnim*>(emitter->animArray + 1);
            if (anim != NULL) // ???
            {
                for (u32 i = 0; i < emitter->animArray->numAnim; i++)
                {
                    if (anim->animValIdx == 17)
                    {
                        found = true;
                        break;
                    }

                    anim = reinterpret_cast<KeyFrameAnim*>((u32)anim + anim->nextOffs);
                }
            }

            if (found)
            {
                ptcl->scaleAnim->startDiff.x = scaleAnimStartDiff.x * emitter->anim[17] * scaleRandom.x;
                ptcl->scaleAnim->startDiff.y = scaleAnimStartDiff.y * emitter->anim[18] * scaleRandom.y;
                ptcl->scaleAnim->endDiff.x = scaleAnimEndDiff.x * emitter->anim[17] * scaleRandom.x;
                ptcl->scaleAnim->endDiff.y = scaleAnimEndDiff.y * emitter->anim[18] * scaleRandom.y;

                ptcl->scale.x = scale.x * emitter->anim[17] * scaleRandom.x;
                ptcl->scale.y = scale.y * emitter->anim[18] * scaleRandom.y;
            }
            else
            {
                ptcl->scaleAnim->startDiff.x = scaleAnimStartDiff.x * data->_5E0.x * scaleRandom.x;
                ptcl->scaleAnim->startDiff.y = scaleAnimStartDiff.y * data->_5E0.y * scaleRandom.y;
                ptcl->scaleAnim->endDiff.x = scaleAnimEndDiff.x * data->_5E0.x * scaleRandom.x;
                ptcl->scaleAnim->endDiff.y = scaleAnimEndDiff.y * data->_5E0.y * scaleRandom.y;

                ptcl->scale.x = scale.x * data->_5E0.x * scaleRandom.x;
                ptcl->scale.y = scale.y * data->_5E0.y * scaleRandom.y;
            }
        }
    }

    ptcl->rotation.x = data->ptclRotate.x + emitter->random.GetF32() * data->ptclRotateRandom.x + emitterSet->_26C.x;
    ptcl->rotation.y = data->ptclRotate.y + emitter->random.GetF32() * data->ptclRotateRandom.y + emitterSet->_26C.y;
    ptcl->rotation.z = data->ptclRotate.z + emitter->random.GetF32() * data->ptclRotateRandom.z + emitterSet->_26C.z;

    ptcl->angularVelocity = (math::VEC3){ 0.0f, 0.0f, 0.0f }; // ???

    ptcl->angularVelocity.x = ptcl->angularVelocity.x + emitter->random.GetF32() * data->angularVelocityRandom.x;
    ptcl->angularVelocity.y = ptcl->angularVelocity.y + emitter->random.GetF32() * data->angularVelocityRandom.y;
    ptcl->angularVelocity.z = ptcl->angularVelocity.z + emitter->random.GetF32() * data->angularVelocityRandom.z;

    if (data->ptclColorSrc[0] == ColorSourceType_Random)
    {
        u32 colorIdx = ptcl->randomU32 % 3;
        ptcl->color[0].rgb() = data->ptclColorTbl[0][colorIdx].rgb();
    }
    else
    {
        u32 colorIdx = 0;
        ptcl->color[0].rgb() = data->ptclColorTbl[0][colorIdx].rgb();
    }

    if (data->ptclColorSrc[1] == ColorSourceType_Random)
    {
        u32 colorIdx = ptcl->randomU32 % 3;
        ptcl->color[1].rgb() = data->ptclColorTbl[1][colorIdx].rgb();
    }
    else
    {
        u32 colorIdx = 0;
        ptcl->color[1].rgb() = data->ptclColorTbl[1][colorIdx].rgb();
    }

    ptcl->texAnimParam[0].scroll.x = data->texAnimParam[0].texInitScroll.x - data->texAnimParam[0].texInitScrollRandom.x * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->texAnimParam[0].scroll.y = data->texAnimParam[0].texInitScroll.y - data->texAnimParam[0].texInitScrollRandom.y * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->texAnimParam[0].scale.x  = data->texAnimParam[0].texInitScale.x  - data->texAnimParam[0].texInitScaleRandom.x  * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->texAnimParam[0].scale.y  = data->texAnimParam[0].texInitScale.y  - data->texAnimParam[0].texInitScaleRandom.y  * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->texAnimParam[0].rotate   = data->texAnimParam[0].texInitRotate   - data->texAnimParam[0].texInitRotateRandom   * emitter->random.GetF32Range(-1.0f, 1.0f);

    ptcl->texAnimParam[1].scroll.x = data->texAnimParam[1].texInitScroll.x - data->texAnimParam[1].texInitScrollRandom.x * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->texAnimParam[1].scroll.y = data->texAnimParam[1].texInitScroll.y - data->texAnimParam[1].texInitScrollRandom.y * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->texAnimParam[1].scale.x  = data->texAnimParam[1].texInitScale.x  - data->texAnimParam[1].texInitScaleRandom.x  * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->texAnimParam[1].scale.y  = data->texAnimParam[1].texInitScale.y  - data->texAnimParam[1].texInitScaleRandom.y  * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->texAnimParam[1].rotate   = data->texAnimParam[1].texInitRotate   - data->texAnimParam[1].texInitRotateRandom   * emitter->random.GetF32Range(-1.0f, 1.0f);

    if (emitter->ptclFollowType == PtclFollowType_SRT)
    {
        ptcl->pMatrixRT = &emitter->matrixRT;
        ptcl->pMatrixSRT = &emitter->matrixSRT;
    }
    else
    {
        math::MTX34::Copy(&ptcl->matrixRT, &emitter->matrixRT);
        math::MTX34::Copy(&ptcl->matrixSRT, &emitter->matrixSRT);
        ptcl->pMatrixRT = &ptcl->matrixRT;
        ptcl->pMatrixSRT = &ptcl->matrixSRT;
    }

    if (data->texAnimParam[0].texPtnAnimNum <= 1)
        ptcl->texAnimParam[0].offset = (math::VEC2){ 0.0f, 0.0f };

    else // TexPtnAnim Type Random
    {
        s32 texPtnAnimIdx = emitter->random.GetU32(data->texAnimParam[0].texPtnAnimNum);
        s32 texPtnAnimIdxDiv = data->texAnimParam[0].texPtnAnimIdxDiv;
        s32 offsetX = texPtnAnimIdx % texPtnAnimIdxDiv;
        s32 offsetY = texPtnAnimIdx / texPtnAnimIdxDiv;

        ptcl->texAnimParam[0].offset.x = data->texAnimParam[0].uvScaleInit.x * (f32)offsetX;
        ptcl->texAnimParam[0].offset.y = data->texAnimParam[0].uvScaleInit.y * (f32)offsetY;
    }

    if (data->texAnimParam[1].texPtnAnimNum <= 1)
        ptcl->texAnimParam[1].offset = (math::VEC2){ 0.0f, 0.0f };

    else // TexPtnAnim Type Random
    {
        s32 texPtnAnimIdx = emitter->random.GetU32(data->texAnimParam[1].texPtnAnimNum);
        s32 texPtnAnimIdxDiv = data->texAnimParam[1].texPtnAnimIdxDiv;
        s32 offsetX = texPtnAnimIdx % texPtnAnimIdxDiv;
        s32 offsetY = texPtnAnimIdx / texPtnAnimIdxDiv;

        ptcl->texAnimParam[1].offset.x = data->texAnimParam[1].uvScaleInit.x * (f32)offsetX;
        ptcl->texAnimParam[1].offset.y = data->texAnimParam[1].uvScaleInit.y * (f32)offsetY;
    }

    if (data->vertexTransformMode == VertexTransformMode_Stripe)
        ptcl->stripe = mSys->AllocAndConnectStripe(emitter, ptcl);

    ptcl->fluctuationAlpha = 1.0f;
    ptcl->fluctuationScale = 1.0f;
    ptcl->_140 = 0;
    ptcl->emitter = emitter;
    ptcl->childEmitCounter = 1000000.0f;
    ptcl->childPreCalcCounter = 0.0f;
    ptcl->childEmitLostTime = 0.0f;
    ptcl->randomF32 = 1.0f - data->_2A4 * emitter->random.GetF32Range(-1.0f, 1.0f);

    CustomActionParticleEmitCallback callback = mSys->GetCurrentCustomActionParticleEmitCallback(emitter);
    if (callback != NULL)
    {
        ParticleEmitArg arg = { .ptcl = ptcl };
        if (!callback(arg))
            return RemoveParticle(emitter, ptcl, CpuCore_1);
    }

    AddPtclToList(emitter, ptcl);
}

} } // namespace nw::eft
