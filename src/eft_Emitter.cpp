#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_Misc.h>
#include <eft_Primitive.h>
#include <eft_Shader.h>
#include <eft_System.h>

namespace nw { namespace eft {

System* EmitterCalc::mSys = NULL;

void EmitterInstance::Init(const SimpleEmitterData* data)
{
    this->data = data;

    emissionInterval = data->emitInterval - random.GetS32(data->emitIntervalRandom);
    fadeStartFrame = -1.0f;
    randomF32 = random.GetF32();
    fadeAlpha = 1.0f;
    emissionSpeed = 1.0f;
    _unused = 0;

    prevPos.x = emitterSet->matrixRT.m[0][3];
    prevPos.y = emitterSet->matrixRT.m[1][3];
    prevPos.z = emitterSet->matrixRT.m[2][3];

    ptclFollowType = data->ptclFollowType;

    math::MTX34::Copy(&animMatrixRT,  &math::MTX34::Identity());
    math::MTX34::Copy(&animMatrixSRT, &math::MTX34::Identity());

    animArray = data->keyAnimArray.ptr;

    if (animArray != NULL)
    {
        KeyFrameAnim* anim = reinterpret_cast<KeyFrameAnim*>(animArray + 1);
        for (u32 i = 0; i < animArray->numAnim; i++)
        {
            if (anim->animValIdx < 27)
            {
                usedAnimArray[numUsedAnim++] = anim;

                if (anim->animValIdx == 8)
                    emitterBehaviorFlg |= EmitterBehaviorFlag_HasTransAnim;

                if (anim->animValIdx == 5)
                    emitterBehaviorFlg |= EmitterBehaviorFlag_HasRotateAnim;

                if (anim->animValIdx == 8 || anim->animValIdx == 5 || anim->animValIdx == 2)
                    emitterBehaviorFlg |= EmitterBehaviorFlag_HasSRTAnim;
            }

            if (anim->animValIdx > 27)
                ptclAnimArray[anim->animValIdx - (27 + 1)] = anim;

            anim = reinterpret_cast<KeyFrameAnim*>((u32)anim + anim->nextOffs);
        }
    }
}

void _copyTextureShiftAnimParm(TexUvShiftAnimUbo* ubo, u32* flag, const TextureEmitterData* texAnimParam, TextureSlot slot, u8 forceRandomType0)
{
    ubo->pm0.zw() = texAnimParam->texInitScroll;
    ubo->pm1.xy() = texAnimParam->texInitScrollRandom;
    ubo->pm0.xy() = texAnimParam->texIncScroll;

    ubo->pm3.y    = texAnimParam->texInitRotate;
    ubo->pm3.z    = texAnimParam->texInitRotateRandom;
    ubo->pm3.x    = texAnimParam->texIncRotate;

    ubo->pm2.xy() = texAnimParam->texInitScale;
    ubo->pm2.zw() = texAnimParam->texInitScaleRandom;
    ubo->pm1.zw() = texAnimParam->texIncScale;

    ubo->pm4.xy() = texAnimParam->uvScaleInit;

    ubo->pm4.z    = (f32)texAnimParam->texPtnAnimIdxDiv[0];
    ubo->pm4.w    = (f32)texAnimParam->texPtnAnimIdxDiv[1];

    if (slot == TextureSlot_0)
    {
        if (texAnimParam->texInvURandom != 0)
            *flag |= 0x0400;

        if (texAnimParam->texInvVRandom != 0)
            *flag |= 0x0800;

        ubo->pm3.w = 0.0f;
    }

    if (slot == TextureSlot_1)
    {
        if (texAnimParam->texInvURandom != 0)
            *flag |= 0x1000;

        if (texAnimParam->texInvVRandom != 0)
            *flag |= 0x2000;

        ubo->pm3.w = 1.0f;
    }

    if (slot == TextureSlot_2)
    {
        if (texAnimParam->texInvURandom != 0)
            *flag |= 0x4000;

        if (texAnimParam->texInvVRandom != 0)
            *flag |= 0x8000;

        ubo->pm3.w = 2.0f;
    }

    if (forceRandomType0 != 0)
        ubo->pm3.w = 0.0f;
}

void _copyTexturePtnAnimParm(TexPtnAnimUbo* ubo, u32* flag, const TextureEmitterData* texAnimParam, TextureSlot slot)
{
    u32 flag_texPtnAnimRandStart;
    u32 flag_texPtnAnimLifeFit;
    u32 flag_texPtnAnimClamp;
    u32 flag_texPtnAnimLoop;
    u32 flag_texPtnAnimRandom;
    u32 flag_texSurfaceNoCrossFade;
    u32 flag_texPtnAnimSurface;

  //if (slot == TextureSlot_0)
    {
        flag_texPtnAnimRandStart   = 0x00000001;
        flag_texPtnAnimLifeFit     = 0x00000004;
        flag_texPtnAnimClamp       = 0x00000010;
        flag_texPtnAnimLoop        = 0x00000020;
        flag_texPtnAnimRandom      = 0x00000008;
        flag_texSurfaceNoCrossFade = 0x00010000;
        flag_texPtnAnimSurface     = 0x10000000;
    }

    if (slot == TextureSlot_1)
    {
        flag_texPtnAnimRandStart   = 0x00000002;
        flag_texPtnAnimLifeFit     = 0x00000040;
        flag_texPtnAnimClamp       = 0x00000100;
        flag_texPtnAnimLoop        = 0x00000200;
        flag_texPtnAnimRandom      = 0x00000080;
        flag_texSurfaceNoCrossFade = 0x00020000;
        flag_texPtnAnimSurface     = 0x20000000;
    }

    if (texAnimParam->texPtnAnimRandStart)
        *flag |= flag_texPtnAnimRandStart;

    switch (texAnimParam->texPtnAnimMode)
    {
    case 1: *flag |= flag_texPtnAnimLifeFit; break;
    case 2: *flag |= flag_texPtnAnimClamp;   break;
    case 3: *flag |= flag_texPtnAnimLoop;    break;
    case 4: *flag |= flag_texPtnAnimRandom;  break;
    case 5: *flag |= flag_texPtnAnimSurface; break;
    }

    if (texAnimParam->texSurfaceNoCrossFade)
        *flag = flag_texSurfaceNoCrossFade;

    ubo->pm.y = (f32)texAnimParam->texPtnAnimPeriod;
    ubo->pm.x = (f32)texAnimParam->texPtnAnimUsedSize;
    ubo->pm.z = (f32)texAnimParam->texPtnAnimNum;

    for (u32 i = 0; i < 32u; i++)
        ubo->tbl[i] = texAnimParam->texPtnAnimData[i];
}

void EmitterInstance::UpdateEmitterStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const SimpleEmitterData* data, const ComplexEmitterData* cdata)
{
    DCZeroRange(uniformBlock, sizeof(EmitterStaticUniformBlock));

    u32 flag = 0;

    _copyTextureShiftAnimParm(&uniformBlock->shtAnim0, &flag, &data->texAnimParam[TextureSlot_0], TextureSlot_0, data->forceRandomType0);
    _copyTextureShiftAnimParm(&uniformBlock->shtAnim1, &flag, &data->texAnimParam[TextureSlot_1], TextureSlot_1, data->forceRandomType0);
    _copyTextureShiftAnimParm(&uniformBlock->shtAnim2, &flag, &data->texAnimParam[TextureSlot_2], TextureSlot_2, data->forceRandomType0);

    _copyTexturePtnAnimParm(&uniformBlock->ptnAnm0, &flag, &data->texAnimParam[TextureSlot_0], TextureSlot_0);
    _copyTexturePtnAnimParm(&uniformBlock->ptnAnm1, &flag, &data->texAnimParam[TextureSlot_1], TextureSlot_1);

    if (data->rotateDirRandomX != 0)
        flag |= 0x0040000;

    if (data->rotateDirRandomY != 0)
        flag |= 0x0080000;

    if (data->rotateDirRandomZ != 0)
        flag |= 0x0100000;

    if (data->primitiveScaleYToZ != 0)
        flag |= 0x8000000;

    uniformBlock->rotBasis.xy() = data->rotBasis;
    uniformBlock->rotBasis.z = data->cameraOffset;
    uniformBlock->rotBasis.w = 0.0f;

    uniformBlock->u_fresnelMinMax.xy() = data->fresnelMinMax;
    uniformBlock->u_fresnelMinMax.z = data->fragmentSoftEdgeAlphaOffset;
    uniformBlock->u_fresnelMinMax.w = data->decalVolume;

    uniformBlock->u_nearAlpha.xy() = data->nearAlphaMinMax;
    uniformBlock->u_nearAlpha.zw() = data->farAlphaMinMax;

    uniformBlock->rotateVel.xyz() = data->angularVelocity;
    uniformBlock->rotateVel.w = data->rotInertia;

    uniformBlock->rotateVelRandom.xyz() = data->angularVelocityRandom;
    uniformBlock->rotateVelRandom.w = 0.0f;

    uniformBlock->alphaAnim0.x = data->alphaAnim[0].startValue;
    uniformBlock->alphaAnim0.y = data->alphaAnim[0].startValue + data->alphaAnim[0].startDiff;
    uniformBlock->alphaAnim0.z = data->alphaAnim[0].time2;
    uniformBlock->alphaAnim0.w = 0.0f;
    uniformBlock->alphaAnim1.x = data->alphaAnim[0].startValue + data->alphaAnim[0].startDiff + data->alphaAnim[0].endDiff;
    uniformBlock->alphaAnim1.y = data->alphaAnim[0].time3;

    uniformBlock->alphaAnim1.z = data->alphaAnim[1].startValue;
    uniformBlock->alphaAnim1.w = data->alphaAnim[1].startValue + data->alphaAnim[1].startDiff;
    uniformBlock->alphaAnim2.x = data->alphaAnim[1].time2;
  //uniformBlock->alphaAnim2   = 0.0f;
    uniformBlock->alphaAnim2.y = data->alphaAnim[1].startValue + data->alphaAnim[1].startDiff + data->alphaAnim[1].endDiff;
    uniformBlock->alphaAnim2.z = data->alphaAnim[1].time3;

    uniformBlock->alphaAnim2.w = (f32)data->colorNumRepetition[0];

    uniformBlock->scaleAnim0.x = data->ptclEmitScale.x;
    uniformBlock->scaleAnim0.y = data->ptclEmitScale.y;
    uniformBlock->scaleAnim0.z = data->ptclScaleStart.x;
    uniformBlock->scaleAnim0.w = data->ptclScaleStart.y;

    uniformBlock->scaleAnim1.x = (f32)data->scaleAnimTime2 / 100.0f;
    uniformBlock->scaleAnim1.y = data->ptclScaleStart.x + data->ptclScaleStartDiff.x;
    uniformBlock->scaleAnim1.z = data->ptclScaleStart.y + data->ptclScaleStartDiff.y;
    uniformBlock->scaleAnim1.w = data->ptclScaleRandom.x;

    uniformBlock->scaleAnim2.x = (f32)data->scaleAnimTime3 / 100.0f;
    uniformBlock->scaleAnim2.y = data->ptclScaleStart.x + data->ptclScaleStartDiff.x + data->ptclScaleEndDiff.x;
    uniformBlock->scaleAnim2.z = data->ptclScaleStart.y + data->ptclScaleStartDiff.y + data->ptclScaleEndDiff.y;
    uniformBlock->scaleAnim2.w = data->ptclScaleRandom.y;

    for (u32 i = 0 ; i < 8u; i++)
    {
        uniformBlock->col0[i] = data->ptclColorTbl[0][i].v;
        uniformBlock->col1[i] = data->ptclColorTbl[1][i].v;
    }

    uniformBlock->color0Anim.x = (f32)data->colorTime2[0] / 100.0f;
    uniformBlock->color0Anim.y = (f32)data->colorTime3[0] / 100.0f;
    uniformBlock->color0Anim.z = (f32)data->colorTime4[0] / 100.0f;

    uniformBlock->color0Anim.w = data->colorScaleFactor;

    uniformBlock->color1Anim.x = (f32)data->colorTime2[1] / 100.0f;
    uniformBlock->color1Anim.y = (f32)data->colorTime3[1] / 100.0f;
    uniformBlock->color1Anim.z = (f32)data->colorTime4[1] / 100.0f;

    uniformBlock->color1Anim.w = (f32)data->colorNumRepetition[1];

    uniformBlock->vectorParam.x = data->airResist;
    uniformBlock->vectorParam.y = data->dir.x;
    uniformBlock->vectorParam.z = data->dir.y;
    uniformBlock->vectorParam.w = data->dir.z;

    uniformBlock->gravityParam.xyz() = data->gravity;
    uniformBlock->gravityParam.w = data->_7F4;

    if (cdata != NULL)
    {
        if (cdata->fluctuationFlags & 1)
        {
            const FluctuationData* fluctuationData = reinterpret_cast<const FluctuationData*>((u32)cdata + cdata->fluctuationDataOffs);

            uniformBlock->flucX.x = fluctuationData->x.amplitude;
            uniformBlock->flucX.y = 128.0f / fluctuationData->x.frequency;
            uniformBlock->flucX.z = fluctuationData->x.phase;
            uniformBlock->flucX.w = (f32)fluctuationData->x.enableRandom;

            uniformBlock->flucY.x = fluctuationData->y.amplitude;
            uniformBlock->flucY.y = 128.0f / fluctuationData->y.frequency;
            uniformBlock->flucY.z = fluctuationData->y.phase;
            uniformBlock->flucY.w = (f32)fluctuationData->y.enableRandom;

            if (cdata->fluctuationFlags & 0x08)
                flag |= 0x1200000;

            if (cdata->fluctuationFlags & 0x10)
                flag |= 0x2400000;

            if (cdata->fluctuationFlags & 0x20)
                flag |= 0x4800000;
        }

        if (cdata->fieldFlags != 0)
        {
            const void* fieldData = reinterpret_cast<const void*>((u32)cdata + cdata->fieldDataOffs);

            if (cdata->fieldFlags & 1)
            {
                const FieldRandomData* randomData = static_cast<const FieldRandomData*>(fieldData);

                uniformBlock->fieldRandomParam.xyz() = randomData->randomVelScale;
                uniformBlock->fieldRandomParam.w = (f32)randomData->period;

                fieldData = randomData + 1;
            }

            if (cdata->fieldFlags & 2)
            {
                const FieldMagnetData* magnetData = static_cast<const FieldMagnetData*>(fieldData);

                uniformBlock->fieldMagnetParam.xyz() = magnetData->pos;
                uniformBlock->fieldMagnetParam.w = magnetData->strength;

                if (magnetData->followEmitter != 0)
                    uniformBlock->fieldCoordEmitter.y = 1.0f;

                fieldData = magnetData + 1;
            }

            if (cdata->fieldFlags & 4)
            {
                const FieldSpinData* spinData = static_cast<const FieldSpinData*>(fieldData);

                uniformBlock->fieldSpinParam.x = nw::math::Idx2Rad(spinData->angle);
                uniformBlock->fieldSpinParam.y = spinData->diffusionVel;
                uniformBlock->fieldSpinParam.z = (f32)spinData->axis;

                fieldData = spinData + 1;
            }

            if (cdata->fieldFlags & 8)
            {
                const FieldCollisionData* collisionData = static_cast<const FieldCollisionData*>(fieldData);

                // The ultimate code
                u32 collisionType = 0;
                if (collisionData->collisionType == 1)
                    collisionType = 2;

                if (collisionData->coordSystem != 0)
                    collisionType++;

                if (collisionData->collisionType == 2)
                {
                    collisionType = 4;
                    if (collisionData->coordSystem != 0)
                        collisionType++;
                }

                // bounceCount is unused?
                uniformBlock->fieldCollisionParam.x = (f32)collisionType;
                uniformBlock->fieldCollisionParam.y = collisionData->bounceRate;
                uniformBlock->fieldCollisionParam.z = collisionData->friction;
                uniformBlock->fieldCollisionParam.w = collisionData->y;

                fieldData = collisionData + 1;
            }

            if (cdata->fieldFlags & 0x10)
            {
                const FieldConvergenceData* convergenceData = static_cast<const FieldConvergenceData*>(fieldData);

                uniformBlock->fieldConvergenceParam.xyz() = convergenceData->pos;
                uniformBlock->fieldConvergenceParam.w = convergenceData->strength;

                fieldData = convergenceData + 1;

                if (convergenceData->followType == 1)
                    uniformBlock->fieldCoordEmitter.x = 1.0f;
            }

            if (cdata->fieldFlags & 0x20)
            {
                const FieldPosAddData* posAddData = static_cast<const FieldPosAddData*>(fieldData);

                uniformBlock->fieldPosAddParam.xyz() = posAddData->posAdd;
                uniformBlock->fieldPosAddParam.w = (f32)posAddData->coordSystem;

                fieldData = posAddData + 1;
            }

            if (cdata->fieldFlags & 0x40)
            {
                const FieldCurlNoiseData* curlNoiseData = static_cast<const FieldCurlNoiseData*>(fieldData);

                uniformBlock->fieldCurlNoise0.xyz() = curlNoiseData->weight;
                uniformBlock->fieldCurlNoise0.w = curlNoiseData->scale;
                uniformBlock->fieldCurlNoise1.xyz() = curlNoiseData->speed;
                uniformBlock->fieldCurlNoise1.w = curlNoiseData->offset;

                fieldData = curlNoiseData + 1;
            }
        }
    }

    uniformBlock->u_shaderParam.xy() = data->shaderParam01;
    uniformBlock->u_shaderParam.z = data->fragmentSoftEdgeFadeDist;
    uniformBlock->u_shaderParam.w = data->fragmentSoftEdgeVolume;

    uniformBlock->u_shaderParamAnim0.x = data->shaderParamAnim.startValue;
    uniformBlock->u_shaderParamAnim0.y = data->shaderParamAnim.startValue + data->shaderParamAnim.startDiff;
    uniformBlock->u_shaderParamAnim0.z = data->shaderParamAnim.time2;
    uniformBlock->u_shaderParamAnim0.w = (f32)data->enableShaderParamAnim;
    uniformBlock->u_shaderParamAnim1.x = data->shaderParamAnim.startValue + data->shaderParamAnim.startDiff + data->shaderParamAnim.endDiff;
    uniformBlock->u_shaderParamAnim1.y = data->shaderParamAnim.time3;
    uniformBlock->u_shaderParamAnim1.z = 0.0f;
    uniformBlock->u_shaderParamAnim1.w = 0.0f;

    uniformBlock->flag[0] = flag;

    GX2EndianSwap(uniformBlock, sizeof(EmitterStaticUniformBlock));
    DCFlushRange(uniformBlock, sizeof(EmitterStaticUniformBlock));
}

void EmitterInstance::UpdateChildStaticUniformBlock(EmitterStaticUniformBlock* uniformBlock, const ChildData* data)
{
    DCZeroRange(uniformBlock, sizeof(EmitterStaticUniformBlock));

    uniformBlock->rotBasis.xy() = data->rotBasis;
    uniformBlock->rotBasis.z = 0.0f;
    uniformBlock->rotBasis.w = 0.0f;

    uniformBlock->u_fresnelMinMax.xy() = data->fresnelMinMax;
    uniformBlock->u_fresnelMinMax.z = data->fragmentSoftEdgeAlphaOffset;
    uniformBlock->u_fresnelMinMax.w = data->decalVolume;

    uniformBlock->u_nearAlpha.xy() = data->nearAlphaMinMax;
    uniformBlock->u_nearAlpha.zw() = data->farAlphaMinMax;

    u32 flag = 0;

    _copyTextureShiftAnimParm(&uniformBlock->shtAnim0, &flag, &data->texAnimParam, TextureSlot_0, 0);
    uniformBlock->shtAnim0.pm2.xy() = (math::VEC2){ 1.0f, 1.0f };

    _copyTexturePtnAnimParm(&uniformBlock->ptnAnm0, &flag, &data->texAnimParam, TextureSlot_0);

    if (data->primitiveScaleYToZ != 0)
        flag |= 0x8000000;

    uniformBlock->flag[0] = flag;

    uniformBlock->u_shaderParam.xy() = data->shaderParam01;
    uniformBlock->u_shaderParam.z = data->fragmentSoftEdgeFadeDist;
    uniformBlock->u_shaderParam.w = data->fragmentSoftEdgeVolume;

    uniformBlock->u_shaderParamAnim0.x = data->shaderParamAnim.startValue;
    uniformBlock->u_shaderParamAnim0.y = data->shaderParamAnim.startValue + data->shaderParamAnim.startDiff;
    uniformBlock->u_shaderParamAnim0.z = data->shaderParamAnim.time2;
    uniformBlock->u_shaderParamAnim0.w = (f32)data->enableShaderParamAnim;
    uniformBlock->u_shaderParamAnim1.x = data->shaderParamAnim.startValue + data->shaderParamAnim.startDiff + data->shaderParamAnim.endDiff;
    uniformBlock->u_shaderParamAnim1.y = data->shaderParamAnim.time3;
    uniformBlock->u_shaderParamAnim1.z = 0.0f;
    uniformBlock->u_shaderParamAnim1.w = 0.0f;

    GX2EndianSwap(uniformBlock, sizeof(EmitterStaticUniformBlock));
    DCFlushRange(uniformBlock, sizeof(EmitterStaticUniformBlock));
}

void EmitterInstance::UpdateEmitterInfoByEmit()
{

    scaleRandom.x = 0.0f;
    scaleRandom.y = 0.0f;
    scaleRandom.z = 0.0f;

    if (emitterBehaviorFlg & EmitterBehaviorFlag_HasRotateAnim)
    {
        rotateRandom.x = 0.0f;
        rotateRandom.y = 0.0f;
        rotateRandom.z = 0.0f;
    }
    else
    {
        rotateRandom.x = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.x;
        rotateRandom.y = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.y;
        rotateRandom.z = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.z;
    }

    if (emitterBehaviorFlg & EmitterBehaviorFlag_HasTransAnim)
    {
        translateRandom.x = 0.0f;
        translateRandom.y = 0.0f;
        translateRandom.z = 0.0f;
    }
    else
    {
        translateRandom.x = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.x;
        translateRandom.y = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.y;
        translateRandom.z = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.z;
    }

    math::VEC3 scale, rotate, translate;
    math::VEC3::Add(&scale, &data->emitterScale, &scaleRandom);
    math::VEC3::Add(&rotate, &data->emitterRotate, &rotateRandom);
    math::VEC3::Add(&translate, &data->emitterTranslate, &translateRandom);

    math::MTX34::MakeSRT(&animMatrixSRT, &scale, &rotate, &translate);
    math::MTX34::MakeRT(&animMatrixRT, &rotate, &translate);

    math::MTX34::Concat(&matrixRT,  &emitterSet->matrixRT,  &animMatrixRT);
    math::MTX34::Concat(&matrixSRT, &emitterSet->matrixSRT, &animMatrixSRT);
}

void EmitterInstance::UpdateResInfo()
{
    const ComplexEmitterData* cdata = GetComplexEmitterData();

    bool gpuCalc   = shader[ShaderType_Normal]->vertexShaderKey.flags[0] & 0x2000000;
    bool streamOut = shader[ShaderType_Normal]->vertexShaderKey.flags[0] & 0x8000000;

    ptclAttributeBuffer = NULL;
    childPtclAttributeBuffer = NULL;

    if (gpuCalc)
    {
        s32 numEmit;
        f32 emissionRate = data->emissionRate;

        if (data->emitFunction == 15 && volumePrimitive != NULL && data->primitiveEmitType == 0)
            emissionRate = (f32)(volumePrimitive->vbPos.bufferSize / sizeof(math::VEC3));

        if (data->oneTime != 0)
            numEmit = (s32)((f32)((data->endFrame - data->startFrame) / (data->emitInterval + 1) + 1) * emissionRate);

        else
            numEmit = (s32)((f32)(data->ptclMaxLifespan / (data->emitInterval + 1)) * emissionRate) + (s32)emissionRate;

        if (numEmit != 0)
        {
            if (ptclAttributeBufferGpu != NULL && numPtclAttributeBufferGpuMax < numEmit)
            {
                FreeFromDynamicHeap(ptclAttributeBufferGpu, true);
                ptclAttributeBufferGpu = NULL;
            }

            if (ptclAttributeBufferGpu == NULL)
                ptclAttributeBufferGpu = static_cast<PtclAttributeBufferGpu*>(AllocFromDynamicHeap(sizeof(PtclAttributeBufferGpu) * numEmit));

            if (streamOut && numPtclAttributeBufferGpuMax != numEmit)
            {
                posStreamOutAttributeBuffer.Finalize();
                vecStreamOutAttributeBuffer.Finalize();
                posStreamOutAttributeBuffer.Initialize(sizeof(nw::math::VEC4) * numEmit);
                vecStreamOutAttributeBuffer.Initialize(sizeof(nw::math::VEC4) * numEmit);
            }
        }

        numPtclAttributeBufferGpuMax = numEmit;
        currentPtclAttributeBufferGpuIdx = 0;
        numDrawParticle = 0;
    }
    else
    {
        ptclAttributeBufferGpu = NULL;
        numPtclAttributeBufferGpuMax = 0;
    }

    if (cdata != NULL && cdata->fieldFlags & 8)
    {
        const void* fieldData = reinterpret_cast<const void*>((u32)cdata + cdata->fieldDataOffs);

        if (cdata->fieldFlags & 1)
        {
            const FieldRandomData* randomData = static_cast<const FieldRandomData*>(fieldData);
            fieldData = randomData + 1;
        }

        if (cdata->fieldFlags & 2)
        {
            const FieldMagnetData* magnetData = static_cast<const FieldMagnetData*>(fieldData);
            fieldData = magnetData + 1;
        }

        if (cdata->fieldFlags & 4)
        {
            const FieldSpinData* spinData = static_cast<const FieldSpinData*>(fieldData);
            fieldData = spinData + 1;
        }

        const FieldCollisionData* collisionData = static_cast<const FieldCollisionData*>(fieldData);
        fieldCollisionY = collisionData->y;
    }
    else
    {
        fieldCollisionY = 0.0f;
    }

    ptclFollowType = data->ptclFollowType;
    particleBehaviorFlg = data->particleBehaviorFlg;

    anim[ 0] = data->emissionRate;
    anim[ 1] = data->ptclMaxLifespan;
    anim[15] = data->allDirVel;
    anim[16] = data->dirVel;
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
    anim[ 2] = data->emitterScale.x + scaleRandom.x;
    anim[ 3] = data->emitterScale.y + scaleRandom.y;
    anim[ 4] = data->emitterScale.z + scaleRandom.z;
    anim[ 5] = data->emitterRotate.x + rotateRandom.x;
    anim[ 6] = data->emitterRotate.y + rotateRandom.y;
    anim[ 7] = data->emitterRotate.z + rotateRandom.z;
    anim[ 8] = data->emitterTranslate.x + translateRandom.x;
    anim[ 9] = data->emitterTranslate.y + translateRandom.x;
    anim[10] = data->emitterTranslate.z + translateRandom.x;
    anim[25] = 1.0f;
    anim[26] = 1.0f;

    UpdateEmitterStaticUniformBlock(emitterStaticUniformBlock, data, cdata);

    if (HasChild())
    {
        const ChildData* childData = GetChildData();

        particleBehaviorFlg |= ParticleBehaviorFlag_WldPosDf;
        childParticleBehaviorFlg = childData->particleBehaviorFlg;

        UpdateChildStaticUniformBlock(childEmitterStaticUniformBlock, childData);
    }

    UpdateEmitterInfoByEmit();
}

void EmitterCalc::RemoveParticle(PtclInstance* ptcl, CpuCore core)
{
    if (ptcl->complexParam != NULL && ptcl->complexParam->stripe != NULL)
        mSys->AddStripeRemoveList(ptcl->complexParam->stripe, core);

    EmitterInstance* emitter = ptcl->emitter;

    if (ptcl->type == PtclType_Child)
    {
        emitter->numChildParticles--;

        if (ptcl->prev != NULL && ptcl->next != NULL)
        {
            ptcl->prev->next = ptcl->next;
            ptcl->next->prev = ptcl->prev;
        }
        else if (emitter->childParticleHead == ptcl)
        {
            emitter->childParticleHead = ptcl->next;

            if (ptcl->next != NULL)
                ptcl->next->prev = ptcl->prev;

            else
                emitter->childParticleTail = NULL;
        }
        else if (emitter->childParticleTail == ptcl)
        {
            emitter->childParticleTail = ptcl->prev;

            if (ptcl->prev != NULL)
                ptcl->prev->next = ptcl->next;

            else
                emitter->childParticleHead = NULL;
        }
    }
    else
    {
        emitter->numParticles--;

        if (ptcl->prev != NULL && ptcl->next != NULL)
        {
            ptcl->prev->next = ptcl->next;
            ptcl->next->prev = ptcl->prev;
        }
        else if (emitter->particleHead == ptcl)
        {
            emitter->particleHead = ptcl->next;

            if (ptcl->next != NULL)
                ptcl->next->prev = ptcl->prev;

            else
                emitter->particleTail = NULL;
        }
        else if (emitter->particleTail == ptcl)
        {
            emitter->particleTail = ptcl->prev;

            if (ptcl->prev != NULL)
                ptcl->prev->next = ptcl->next;

            else
                emitter->particleHead = NULL;
        }
    }

    CustomActionParticleRemoveCallback callback = mSys->GetCurrentCustomActionParticleRemoveCallback(ptcl->emitter);
    if (callback != NULL)
    {
        ParticleRemoveArg arg = { .ptcl = ptcl };
        callback(arg);
    }

    ptcl->lifespan = 0;
    ptcl->data = NULL;
}

void EmitterCalc::AddPtclToList(EmitterInstance* emitter, PtclInstance* ptcl)
{
    if (emitter->particleHead == NULL)
    {
        emitter->particleHead = ptcl;
        ptcl->next = NULL;
        ptcl->prev = NULL;
    }
    else
    {
        emitter->particleHead->prev = ptcl;
        ptcl->next = emitter->particleHead;
        emitter->particleHead = ptcl;
        ptcl->prev = NULL;
    }

    if (emitter->particleTail == NULL)
        emitter->particleTail = ptcl;

    emitter->numParticles++;
}

f32 _initialize3v4kAnim(AlphaAnim* anim, const anim3v4Key* key, s32 lifespan)
{
    anim->time2 = (s32)(key->time2 * lifespan);
    anim->time3 = (s32)(key->time3 * lifespan);

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
    const ComplexEmitterData* cdata = emitter->GetComplexEmitterData();

    ptcl->data = data;
    ptcl->cdata = cdata;

    const EmitterSet* emitterSet = emitter->emitterSet;

    ptcl->emitter = emitter;
    ptcl->emitStartFrame = emitter->counter;
    ptcl->counter = 0.0f;
    ptcl->counterS32 = 0;
    ptcl->randomU32 = emitter->random.GetU32();
    ptcl->fieldCollisionCounter = 0;
    ptcl->randomF32 = 1.0f - data->momentumRandom * emitter->random.GetF32Range(-1.0f, 1.0f);
    ptcl->_unused = 0;
    ptcl->randomVec4.x = emitter->random.GetF32();
    ptcl->randomVec4.y = emitter->random.GetF32();
    ptcl->randomVec4.z = emitter->random.GetF32();
    ptcl->randomVec4.w = emitter->random.GetF32();
    ptcl->fluctuationAlpha = 1.0f;
    ptcl->fluctuationScale = (math::VEC2){ 1.0f, 1.0f };
    ptcl->complexParam->childEmitCounter = 1000000.0f;
    ptcl->complexParam->childPreCalcCounter = 0.0f;
    ptcl->complexParam->childEmitLostTime = 0.0f;

    if (data->ptclMaxLifespan == 0x7FFFFFFF)
        ptcl->lifespan = 0x7FFFFFFF;

    else
        ptcl->lifespan = (s32)((emitter->anim[1] - emitter->random.GetS32(data->ptclLifespanRandom)) * emitter->controller->life);

    f32 velocityMagRandom = 1.0f - emitter->random.GetF32() * data->dirVelRandom * emitterSet->dirVelRandom;
    f32 velocityMag = emitter->anim[16] * emitterSet->dirVel;

    if (emitter->ptclFollowType != PtclFollowType_SRT)
    {
        math::MTX34::Copy(&ptcl->matrixRT, &emitter->matrixRT);
        math::MTX34::Copy(&ptcl->matrixSRT, &emitter->matrixSRT);
        ptcl->pMatrixRT = &ptcl->matrixRT;
        ptcl->pMatrixSRT = &ptcl->matrixSRT;
    }
    else
    {
        ptcl->pMatrixRT = &emitter->matrixRT;
        ptcl->pMatrixSRT = &emitter->matrixSRT;
    }

    if (data->ptclPosRandom != 0.0f)
        ptcl->pos = emitter->random.GetNormalizedVec3() * data->ptclPosRandom + ptcl->pos;

    if (emitterSet->dirSet != 0)
        ptcl->velocity = (ptcl->velocity + emitterSet->dir * velocityMag) * velocityMagRandom;

    else
    {
        f32 dispersionAngle = data->dispersionAngle;
        if (dispersionAngle == 0.0f)
            ptcl->velocity = (ptcl->velocity + data->dir * velocityMag) * velocityMagRandom;

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
            math::MTX34::MakeVectorRotation(&mtx, &base, &data->dir);

            math::MTX34::PSMultVec(&normalizedVel, &mtx, &normalizedVel);
            ptcl->velocity = (ptcl->velocity + normalizedVel * velocityMag) * velocityMagRandom;
        }
    }

    math::VEC3 randomVec3 = emitter->random.GetVec3();
    ptcl->velocity.x += randomVec3.x * data->diffusionVel.x;
    ptcl->velocity.y += randomVec3.y * data->diffusionVel.y;
    ptcl->velocity.z += randomVec3.z * data->diffusionVel.z;

    if (data->emitterVelInheritRatio != 0.0f)
    {
        math::VEC3 localPosCurr;
        math::VEC3 localPosPrev;

        {
            math::VEC3 currPos = {
                .x = emitter->matrixRT.m[0][3],
                .y = emitter->matrixRT.m[1][3],
                .z = emitter->matrixRT.m[2][3],
            };

            math::MTX34 matrixSRTInv;
            math::MTX34::Inverse(&matrixSRTInv, (emitter->ptclFollowType == PtclFollowType_SRT) ? &emitter->matrixSRT : &ptcl->matrixSRT);
            math::MTX34::PSMultVec(&localPosCurr, &matrixSRTInv, &currPos);
        }

        {
            math::MTX34 matrixSRTInv;
            math::MTX34::Inverse(&matrixSRTInv, (emitter->ptclFollowType == PtclFollowType_SRT) ? &emitter->matrixSRT : &ptcl->matrixSRT);
            math::MTX34::PSMultVec(&localPosPrev, &matrixSRTInv, &emitter->prevPos);
        }

        math::VEC3 posDiff = (localPosCurr - localPosPrev) * data->emitterVelInheritRatio;

        ptcl->velocity += posDiff;
        ptcl->pos -= posDiff;
    }

    math::VEC3 addVelocity;
    math::VEC3::MultMTX(&addVelocity, &emitterSet->addVelocity, &emitterSet->matrixRT);
    ptcl->velocity += addVelocity;

    if (ptcl->velocity.x != 0.0f || ptcl->velocity.y != 0.0f || ptcl->velocity.y != 0.0f)
        ptcl->posDiff = ptcl->velocity;

    else
    {
        ptcl->posDiff.x = 0.0f;
        ptcl->posDiff.y = 0.0001f;
        ptcl->posDiff.z = 0.0f;
    }

    s32 lifespan = ptcl->lifespan - 1;
    math::VEC2 scaleRandom;
    if (data->ptclScaleRandom.x != data->ptclScaleRandom.y)
    {
        scaleRandom.x = (1.0f - data->ptclScaleRandom.x * emitter->random.GetF32()) * emitterSet->ptclEmitScale.x;
        scaleRandom.y = (1.0f - data->ptclScaleRandom.y * emitter->random.GetF32()) * emitterSet->ptclEmitScale.y;
    }
    else
    {
        scaleRandom = (1.0f - data->ptclScaleRandom.x * emitter->random.GetF32()) * emitterSet->ptclEmitScale;
    }

    scaleRandom.x *= emitter->anim[17];
    scaleRandom.y *= emitter->anim[18];

    if (emitter->shader[ShaderType_Normal]->vertexShaderKey.flags[0] & 0x2000000)
        ptcl->scale = scaleRandom;

    else if (lifespan == 0)
    {
        ptcl->scaleAnim->time2 = -1;
        ptcl->scaleAnim->time3 = 0x7FFFFFFF;
        ptcl->scaleAnim->startDiff = math::VEC2::Zero();
        ptcl->scaleAnim->endDiff = math::VEC2::Zero();

        ptcl->scale.x = data->ptclScaleStart.x * scaleRandom.x * data->ptclEmitScale.x;
        ptcl->scale.y = data->ptclScaleStart.y * scaleRandom.y * data->ptclEmitScale.y;
    }
    else
    {
        ptcl->scaleAnim->time2 = 0; // ???
        ptcl->scaleAnim->time3 = 0; // ^^^

        ptcl->scaleAnim->time2 = (data->scaleAnimTime2 * lifespan) / 100;
        ptcl->scaleAnim->time3 = (data->scaleAnimTime3 * lifespan) / 100;

        math::VEC2 scaleAnimStartDiff;
        math::VEC2 scale;
        math::VEC2 scaleAnimEndDiff;

        if (ptcl->scaleAnim->time2 == 0)
            scaleAnimStartDiff = (math::VEC2){ 0.0f, 0.0f };

        else
            scaleAnimStartDiff = data->ptclScaleStartDiff * (1.0f / (f32)ptcl->scaleAnim->time2);

        if (ptcl->scaleAnim->time3 == lifespan)
            scaleAnimEndDiff = (math::VEC2){ 0.0f, 0.0f };

        else
            scaleAnimEndDiff = data->ptclScaleEndDiff * (1.0f / (f32)(lifespan - ptcl->scaleAnim->time3));

        scale = data->ptclScaleStart - scaleAnimStartDiff;

        ptcl->scaleAnim->startDiff.x = scaleAnimStartDiff.x * data->ptclEmitScale.x * scaleRandom.x;
        ptcl->scaleAnim->startDiff.y = scaleAnimStartDiff.y * data->ptclEmitScale.y * scaleRandom.y;
        ptcl->scaleAnim->endDiff.x = scaleAnimEndDiff.x * data->ptclEmitScale.x * scaleRandom.x;
        ptcl->scaleAnim->endDiff.y = scaleAnimEndDiff.y * data->ptclEmitScale.y * scaleRandom.y;

        ptcl->scale.x = scale.x * data->ptclEmitScale.x * scaleRandom.x;
        ptcl->scale.y = scale.y * data->ptclEmitScale.y * scaleRandom.y;
    }

    ptcl->_13C = 0.0f;

    if (data->ptclAlphaSrc[0] == AlphaSourceType_First || data->ptclAlphaSrc[0] == AlphaSourceType_8key)
        ptcl->alpha0 = data->alphaAnim[0].startValue + data->alphaAnim[0].startDiff;

    else if (data->ptclAlphaSrc[0] == AlphaSourceType_3v4k)
        ptcl->alpha0 = _initialize3v4kAnim(ptcl->alphaAnim[0], &data->alphaAnim[0], lifespan);

    if (data->ptclAlphaSrc[1] == AlphaSourceType_First || data->ptclAlphaSrc[1] == AlphaSourceType_8key)
        ptcl->alpha1 = data->alphaAnim[1].startValue + data->alphaAnim[1].startDiff;

    else if (data->ptclAlphaSrc[1] == AlphaSourceType_3v4k)
        ptcl->alpha1 = _initialize3v4kAnim(ptcl->alphaAnim[1], &data->alphaAnim[1], lifespan);

    ptcl->rotation.x = data->ptclRotate.x + emitter->random.GetF32() * data->ptclRotateRandom.x + emitterSet->ptclRotate.x;
    ptcl->rotation.y = data->ptclRotate.y + emitter->random.GetF32() * data->ptclRotateRandom.y + emitterSet->ptclRotate.y;
    ptcl->rotation.z = data->ptclRotate.z + emitter->random.GetF32() * data->ptclRotateRandom.z + emitterSet->ptclRotate.z;

    ptcl->angularVelocity.x = data->angularVelocity.x + emitter->random.GetF32() * data->angularVelocityRandom.x;
    ptcl->angularVelocity.y = data->angularVelocity.y + emitter->random.GetF32() * data->angularVelocityRandom.y;
    ptcl->angularVelocity.z = data->angularVelocity.z + emitter->random.GetF32() * data->angularVelocityRandom.z;

    if (data->rotateDirRandomX != 0 && emitter->random.GetF32() > 0.5f)
    {
        ptcl->velocity.x = -ptcl->velocity.x;
        ptcl->angularVelocity.x = -ptcl->angularVelocity.x;
    }

    if (data->rotateDirRandomY != 0 && emitter->random.GetF32() > 0.5f)
    {
        ptcl->velocity.y = -ptcl->velocity.y;
        ptcl->angularVelocity.y = -ptcl->angularVelocity.y;
    }

    if (data->rotateDirRandomZ != 0 && emitter->random.GetF32() > 0.5f)
    {
        ptcl->velocity.z = -ptcl->velocity.z;
        ptcl->angularVelocity.z = -ptcl->angularVelocity.z;
    }

    if (data->ptclColorSrc[0] == ColorSourceType_Random)
    {
        u32 colorIdx = ptcl->randomVec4.x * (u32)data->colorNumRepetition[0];
        ptcl->color[0].rgb() = data->ptclColorTbl[0][colorIdx].rgb();
    }
    else
    {
        u32 colorIdx = 0;
        ptcl->color[0].rgb() = data->ptclColorTbl[0][colorIdx].rgb();
    }

    if (data->ptclColorSrc[1] == ColorSourceType_Random)
    {
        u32 colorIdx = ptcl->randomVec4.x * (u32)data->colorNumRepetition[1];
        ptcl->color[1].rgb() = data->ptclColorTbl[1][colorIdx].rgb();
    }
    else
    {
        u32 colorIdx = 0;
        ptcl->color[1].rgb() = data->ptclColorTbl[1][colorIdx].rgb();
    }

    ptcl->color[1].a = 1.0f;

    AddPtclToList(emitter, ptcl);

    CustomActionParticleEmitCallback callback = mSys->GetCurrentCustomActionParticleEmitCallback(emitter);
    if (callback != NULL)
    {
        ParticleEmitArg arg = { .ptcl = ptcl };
        if (!callback(arg))
            return RemoveParticle(ptcl, CpuCore_1);
    }
}

u32 _getUnifiedAnimID(u32 animValIdx)
{
    if (0x26 <= animValIdx && animValIdx <= 0x2B) // Ptcl Color0/1 RGB -> Ptcl Color1 R
        return 0x29;

    if (0x2E <= animValIdx && animValIdx <= 0x2F) // Ptcl Scale XY -> Ptcl Scale X
        return 0x2E;

    return animValIdx;
}

f32 _calcParticleAnimTime(EmitterInstance* emitter, PtclInstance* ptcl, u32 animValIdx)
{
    KeyFrameAnim* anim = emitter->ptclAnimArray[animValIdx - (27 + 1)];
    f32 time = (anim->loop != 0) ? ptcl->counterS32 % (anim->loopLength + 1) * 100.0f / anim->loopLength + (ptcl->counter - ptcl->counterS32)
                                 : 100.0f * ptcl->counter / ptcl->lifespan;

    if (anim->randomStart != 0)
    {
        time += ptcl->randomU32 * _getUnifiedAnimID(animValIdx) * (1.0f / 4294967296.0f) * 100.0f;
        if (time > 100.0f)
            time -= 100.0f;
    }

    return time;
}

f32 _calcParticleAnimTime(KeyFrameAnim* anim, PtclInstance* ptcl, u32 animValIdx)
{
    f32 time = (anim->loop != 0) ? ptcl->counterS32 % (anim->loopLength + 1) * 100.0f / anim->loopLength + (ptcl->counter - ptcl->counterS32)
                                 : 100.0f * ptcl->counter / ptcl->lifespan;

    if (anim->randomStart != 0)
    {
        time += ptcl->randomU32 * _getUnifiedAnimID(animValIdx) * (1.0f / 4294967296.0f) * 100.0f;
        if (time > 100.0f)
            time -= 100.0f;
    }

    return time;
}

} } // namespace nw::eft
