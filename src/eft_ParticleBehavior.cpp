#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

// No division-by-zero checks whatsoever...

static inline void ColorAnim_4k3v(u32 colorIdx, PtclInstance* ptcl, const SimpleEmitterData* data)
{
    s32 period = ptcl->lifespan / data->colorNumRepetition[colorIdx];
    if (period == 0)
        period = ptcl->lifespan;

    s32 counter = (s32)ptcl->counter - 1;
    if (data->colorRandomStart[colorIdx])
        counter += ptcl->randomU32 >> 6;
    counter %= period;

    s32 time2 = (data->colorTime2[colorIdx] * period) / 100;
    if (counter < time2)
        ptcl->color[colorIdx].rgb() = data->ptclColorTbl[colorIdx][0].rgb();

    else
    {
        s32 time3 = (data->colorTime3[colorIdx] * period) / 100;
        if (counter < time3)
            ptcl->color[colorIdx].rgb() = data->ptclColorTbl[colorIdx][0].rgb() + (data->ptclColorTbl[colorIdx][1].rgb() - data->ptclColorTbl[colorIdx][0].rgb()) * ((f32)(counter - time2) / (f32)(time3 - time2));

        else
        {
            s32 time4 = (data->colorTime4[colorIdx] * period) / 100;
            if (counter < time4)
                ptcl->color[colorIdx].rgb() = data->ptclColorTbl[colorIdx][1].rgb() + (data->ptclColorTbl[colorIdx][2].rgb() - data->ptclColorTbl[colorIdx][1].rgb()) * ((f32)(counter - time3) / (f32)(time4 - time3));

            else
                ptcl->color[colorIdx].rgb() = data->ptclColorTbl[colorIdx][2].rgb();
        }
    }
}

static inline void TexPtnAnim(u32 texIdx, PtclInstance* ptcl, const SimpleEmitterData* data)
{
    s32 period = data->texAnimParam[texIdx].texPtnAnimPeriod;
    s16 size = data->texAnimParam[texIdx].texPtnAnimUsedSize;
    s32 counter = (s32)ptcl->counter;
    u32 idx;

    // Four animation types:
    // 1. Fit to lifespan
    // 2. Clamp
    // 3. Loop
    // 4. Random (Not handled here; handled in Emit function)

    //const bool fitToLifespan = period == 0;
    //const bool clamp         = data->texAnimParam[texIdx].texPtnAnimClamp;
    //const bool loop          = !fitToLifespan && !clamp;

    if (period == 0)
        idx = (counter * size) / ptcl->lifespan; // (time / lifespan) * size
    else
        idx = counter / period;                  //  time / period

    if (data->texAnimParam[texIdx].texPtnAnimClamp && idx >= size)
        return;

    if (period != 0 && !data->texAnimParam[texIdx].texPtnAnimClamp
        && data->texAnimParam[texIdx].texPtnAnimRandStart)
    {
        idx += ptcl->randomU32;
    }

    s32 texPtnAnimIdxDiv = data->texAnimParam[texIdx].texPtnAnimIdxDiv;
    s32 texPtnAnimIdx = data->texAnimParam[texIdx].texPtnAnimData[idx % size];
    s32 offsetX = texPtnAnimIdx % texPtnAnimIdxDiv;
    s32 offsetY = texPtnAnimIdx / texPtnAnimIdxDiv;

    ptcl->texAnimParam[texIdx].offset.x = data->texAnimParam[texIdx].uvScaleInit.x * (f32)offsetX;
    ptcl->texAnimParam[texIdx].offset.y = data->texAnimParam[texIdx].uvScaleInit.y * (f32)offsetY;
}

static inline void ChildTexPtnAnim(PtclInstance* ptcl, u32 childFlags, const ChildData* childData)
{
    s32 period = childData->texPtnAnimPeriod;
    s16 size = childData->texPtnAnimUsedSize;
    s32 counter = (s32)ptcl->counter;
    u32 idx;

    // Four animation types:
    // 1. Fit to lifespan
    // 2. Clamp
    // 3. Loop
    // 4. Random (Not handled here; handled in Emit function)

    //const bool fitToLifespan = period == 0;
    //const bool clamp         = childData->texPtnAnimClamp;
    //const bool loop          = !fitToLifespan && !clamp;

    if (period == 0)
        idx = (counter * size) / ptcl->lifespan; // (time / lifespan) * size
    else
        idx = counter / period;                  //  time / period

    if (childData->texPtnAnimClamp && idx >= size)
        return;

    if (period != 0 && !childData->texPtnAnimClamp
        && childFlags & 0x4000)
    {
        idx += ptcl->randomU32;
    }

    s32 texPtnAnimIdxDiv = childData->texPtnAnimIdxDiv;
    s32 texPtnAnimIdx = childData->texPtnAnimData[idx % size];
    s32 offsetX = texPtnAnimIdx % texPtnAnimIdxDiv;
    s32 offsetY = texPtnAnimIdx / texPtnAnimIdxDiv;

    ptcl->texAnimParam[0].offset.x = childData->uvScaleInit.x * (f32)offsetX;
    ptcl->texAnimParam[0].offset.y = childData->uvScaleInit.y * (f32)offsetY;
}

static inline void TexUvShiftAnim(u32 texIdx, PtclInstance* ptcl, const SimpleEmitterData* data, f32 emissionSpeed)
{
    ptcl->texAnimParam[texIdx].scroll.x += data->texAnimParam[texIdx].texIncScroll.x * emissionSpeed;
    ptcl->texAnimParam[texIdx].scroll.y += data->texAnimParam[texIdx].texIncScroll.y * emissionSpeed;
    ptcl->texAnimParam[texIdx].scale.x  += data->texAnimParam[texIdx].texIncScale.x  * emissionSpeed;
    ptcl->texAnimParam[texIdx].scale.y  += data->texAnimParam[texIdx].texIncScale.y  * emissionSpeed;
    ptcl->texAnimParam[texIdx].rotate   += data->texAnimParam[texIdx].texIncRotate   * emissionSpeed;
}

static inline void CalcParitcleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, const SimpleEmitterData* data)
{
    s32 counter = (s32)ptcl->counter;
    f32 emissionSpeedInv = 1.0f - emitter->emissionSpeed;

    math::MTX34* matrixRT = ptcl->pMatrixRT;

    if (emitter->ptclFollowType == PtclFollowType_Trans)
    {
        ptcl->matrixSRT.m[0][3] = emitter->matrixSRT.m[0][3];
        ptcl->matrixSRT.m[1][3] = emitter->matrixSRT.m[1][3];
        ptcl->matrixSRT.m[2][3] = emitter->matrixSRT.m[2][3];
        ptcl->matrixRT.m[0][3] = emitter->matrixRT.m[0][3];
        ptcl->matrixRT.m[1][3] = emitter->matrixRT.m[1][3];
        ptcl->matrixRT.m[2][3] = emitter->matrixRT.m[2][3];
    }

    ptcl->pos += ptcl->velocity * ptcl->randomF32 * emitter->emissionSpeed;

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_AirResist)
        ptcl->velocity *= data->airResist + (1.0f - data->airResist) * emissionSpeedInv;

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Gravity)
    {
        math::VEC3 gravity = data->gravity * emitter->emissionSpeed;

        if (data->transformGravity != 0)
            ptcl->velocity += *math::VEC3::MultMTX(&gravity, &gravity, matrixRT);

        else
            ptcl->velocity += gravity;
    }

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_AlphaAnim)
    {
        // EmitterCalc::ptclAnim_Alpha0_4k3v(EmitterInstance*, PtclInstance*, f32)
        {
            if (counter <= ptcl->alphaAnim->time2)
                ptcl->alpha += ptcl->alphaAnim->startDiff * emitter->emissionSpeed;

            else if (counter > ptcl->alphaAnim->time3)
                ptcl->alpha += ptcl->alphaAnim->endDiff * emitter->emissionSpeed;
        }
    }

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_ScaleAnim)
    {
        // EmitterCalc::ptclAnim_Scale_4k3v(EmitterInstance*, PtclInstance*, f32)
        {
            if (counter <= ptcl->scaleAnim->time2)
            {
                ptcl->scale.x += ptcl->scaleAnim->startDiff.x * emitter->emissionSpeed;
                ptcl->scale.y += ptcl->scaleAnim->startDiff.y * emitter->emissionSpeed;
            }
            else if (counter > ptcl->scaleAnim->time3)
            {
                ptcl->scale.x += ptcl->scaleAnim->endDiff.x * emitter->emissionSpeed;
                ptcl->scale.y += ptcl->scaleAnim->endDiff.y * emitter->emissionSpeed;
            }
        }
    }

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Rotate)
        ptcl->rotation += ptcl->angularVelocity * emitter->emissionSpeed;

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_RotInertia)
        math::VEC3::Scale(&ptcl->angularVelocity, &ptcl->angularVelocity, data->rotInertia + (1.0f - data->rotInertia) * emissionSpeedInv);

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Color0Anim)
    {
        // EmitterCalc::ptclAnim_Color0_4k3v(EmitterInstance*, PtclInstance*, f32)
        ColorAnim_4k3v(0, ptcl, data);
    }

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Color1Anim)
    {
        // EmitterCalc::ptclAnim_Color1_4k3v(EmitterInstance*, PtclInstance*, f32)
        ColorAnim_4k3v(1, ptcl, data);
    }

    {
        if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Tex0PtnAnim)
            TexPtnAnim(0, ptcl, data);

        if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Tex0UVShiftAnim)
            TexUvShiftAnim(0, ptcl, data, emitter->emissionSpeed);
    }

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_HasTex1)
    {
        if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Tex1PtnAnim)
            TexPtnAnim(1, ptcl, data);

        if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Tex1UVShiftAnim)
            TexUvShiftAnim(1, ptcl, data, emitter->emissionSpeed);
    }
}

u32 EmitterCalc::CalcSimpleParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, CpuCore core)
{
    const SimpleEmitterData* data = emitter->data;

    math::VEC3 posBefore = ptcl->pos;
    math::MTX34* matrixSRT = ptcl->pMatrixSRT;

    CalcParitcleBehavior(emitter, ptcl, data);

    math::MTX34::MultVec(&ptcl->worldPos, matrixSRT, &ptcl->pos);

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_WldPosDf)
    {
        math::VEC3 posDiff = ptcl->pos - posBefore;
        if (fabsf(posDiff.x) > 0.0001 || fabsf(posDiff.y) > 0.0001 || fabsf(posDiff.z) > 0.0001)
            ptcl->posDiff += posDiff - ptcl->posDiff;

        *math::MTX34::MultVecSR(&ptcl->worldPosDiff, matrixSRT, &ptcl->posDiff) *= emitter->emissionSpeed;
    }

    ptcl->counter += emitter->emissionSpeed;
    return 0;
}

u32 EmitterCalc::CalcComplexParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, CpuCore core)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);

    math::VEC3 posBefore = ptcl->pos;
    math::MTX34* matrixSRT = ptcl->pMatrixSRT;

    CalcParitcleBehavior(emitter, ptcl, data);

    if (data->fluctuationFlags & 1)
        CalcFluctuation(emitter, ptcl);

    if (data->fieldFlags != 0)
    {
        const void* fieldData = reinterpret_cast<const void*>((u32)data + data->fieldDataOffs);

        if (data->fieldFlags & 0x01) fieldData = _ptclField_Random     (emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x02) fieldData = _ptclField_Magnet     (emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x04) fieldData = _ptclField_Spin       (emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x08) fieldData = _ptclField_Collision  (emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x10) fieldData = _ptclField_Convergence(emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x20) fieldData = _ptclField_PosAdd     (emitter, ptcl, fieldData);
    }

    math::VEC3 posDiff = ptcl->pos - posBefore;
    if (fabsf(posDiff.x) > 0.0001 || fabsf(posDiff.y) > 0.0001 || fabsf(posDiff.z) > 0.0001)
        ptcl->posDiff += posDiff - ptcl->posDiff;

    math::MTX34::MultVec(&ptcl->worldPos, matrixSRT, &ptcl->pos);
    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_WldPosDf)
        *math::MTX34::MultVecSR(&ptcl->worldPosDiff, matrixSRT, &ptcl->posDiff) *= emitter->emissionSpeed;

    ptcl->counter += emitter->emissionSpeed;
    return 0;
}

u32 EmitterCalc::CalcChildParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);
    const ChildData* childData = reinterpret_cast<const ChildData*>(data + 1);

    math::VEC3 posBefore = ptcl->pos;

    math::MTX34* matrixRT = ptcl->pMatrixRT;
    math::MTX34* matrixSRT = ptcl->pMatrixSRT;

    s32 counter = (s32)ptcl->counter;
    f32 emissionSpeedInv = 1.0f - emitter->emissionSpeed;

    ptcl->pos += ptcl->velocity * ptcl->randomF32 * emitter->emissionSpeed;

    //if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_AirResist)
        ptcl->velocity *= childData->airResist + (1.0f - data->airResist) * emissionSpeedInv; // No idea why it uses data->airResist, mistake?

    //if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Gravity)
    {
        math::VEC3 gravity = childData->gravity * emitter->emissionSpeed;

        if (data->transformGravity != 0)
            ptcl->velocity += *math::VEC3::MultMTX(&gravity, &gravity, matrixRT);

        else
            ptcl->velocity += gravity;
    }

    //if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_AlphaAnim)
    {
        if (counter < childData->alphaAnimTime2)
            ptcl->alpha += ptcl->alphaAnim->startDiff * emitter->emissionSpeed;

        else if (counter >= childData->alphaAnimTime3)
            ptcl->alpha += ptcl->alphaAnim->endDiff * emitter->emissionSpeed;
    }

    //if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_ScaleAnim)
    {
        if (counter >= childData->scaleAnimTime1)
        {
            ptcl->scale.x += ptcl->scaleAnim->startDiff.x * emitter->emissionSpeed;
            ptcl->scale.y += ptcl->scaleAnim->startDiff.y * emitter->emissionSpeed;
        }
    }

    //if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Rotate)
        ptcl->rotation += ptcl->angularVelocity * emitter->emissionSpeed;

    //if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_RotInertia)
        math::VEC3::Scale(&ptcl->angularVelocity, &ptcl->angularVelocity, childData->rotInertia + (1.0f - childData->rotInertia) * emissionSpeedInv);

    if (data->childFlags & 0x2000)
        ChildTexPtnAnim(ptcl, data->childFlags, childData);

    if (data->childFlags & 0x800)
    {
        const void* fieldData = reinterpret_cast<const void*>((u32)data + data->fieldDataOffs);

        if (data->fieldFlags & 0x01) fieldData = _ptclField_Random     (emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x02) fieldData = _ptclField_Magnet     (emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x04) fieldData = _ptclField_Spin       (emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x08) fieldData = _ptclField_Collision  (emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x10) fieldData = _ptclField_Convergence(emitter, ptcl, fieldData);
        if (data->fieldFlags & 0x20) fieldData = _ptclField_PosAdd     (emitter, ptcl, fieldData);
    }

    math::VEC3 posDiff = ptcl->pos - posBefore;
    if (fabsf(posDiff.x) > 0.0001 || fabsf(posDiff.y) > 0.0001 || fabsf(posDiff.z) > 0.0001)
        ptcl->posDiff += posDiff - ptcl->posDiff;

    math::MTX34::MultVec(&ptcl->worldPos, &ptcl->matrixSRT, &ptcl->pos);
    //if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_WldPosDf)
        *math::MTX34::MultVecSR(&ptcl->worldPosDiff, matrixSRT, &ptcl->posDiff) *= emitter->emissionSpeed;

    ptcl->counter += emitter->emissionSpeed;
    return 0;
}

void EmitterCalc::MakeParticleAttributeBuffer(PtclAttributeBuffer* ptclAttributeBuffer, PtclInstance* ptcl, u32 shaderAvailableAttribFlg, f32 cameraOffset)
{
    const EmitterSet* emitterSet = ptcl->emitter->emitterSet;

    ptclAttributeBuffer->wldPos.xyz() = ptcl->worldPos;
    ptclAttributeBuffer->wldPos.w = cameraOffset;

    ptclAttributeBuffer->scl.x = ptcl->scale.x * emitterSet->ptclEffectiveScale.x * ptcl->fluctuationScale;
    ptclAttributeBuffer->scl.y = ptcl->scale.y * emitterSet->ptclEffectiveScale.y * ptcl->fluctuationScale;
    ptclAttributeBuffer->scl.z = ptcl->texAnimParam[0].rotate;
    ptclAttributeBuffer->scl.w = ptcl->texAnimParam[1].rotate;

    ptclAttributeBuffer->color0.xyz() = ptcl->color0.rgb();
    ptclAttributeBuffer->color0.w = ptcl->alpha * ptcl->fluctuationAlpha;

    if (shaderAvailableAttribFlg & ShaderAttrib_Color1)
    {
        ptclAttributeBuffer->color1.xyz() = ptcl->color1.rgb();
        ptclAttributeBuffer->color1.w = ptcl->alpha * ptcl->fluctuationAlpha;
    }

    ptclAttributeBuffer->texAnim.x    = ptcl->texAnimParam[0].offset.x + ptcl->texAnimParam[0].scroll.x;
    ptclAttributeBuffer->texAnim.y    = ptcl->texAnimParam[0].offset.y - ptcl->texAnimParam[0].scroll.y;
    ptclAttributeBuffer->texAnim.zw() = ptcl->texAnimParam[0].scale;

    if (shaderAvailableAttribFlg & ShaderAttrib_WldPosDf)
        ptclAttributeBuffer->wldPosDf.xyz() = ptcl->worldPosDiff;

    if (shaderAvailableAttribFlg & ShaderAttrib_Rot)
    {
        ptclAttributeBuffer->rot.xyz() = ptcl->rotation;
        ptclAttributeBuffer->rot.w = 0.0f;
    }

    if (shaderAvailableAttribFlg & ShaderAttrib_SubTexAnim)
    {
        ptclAttributeBuffer->subTexAnim.x    = ptcl->texAnimParam[1].offset.x + ptcl->texAnimParam[1].scroll.x;
        ptclAttributeBuffer->subTexAnim.y    = ptcl->texAnimParam[1].offset.y - ptcl->texAnimParam[1].scroll.y;
        ptclAttributeBuffer->subTexAnim.zw() = ptcl->texAnimParam[1].scale;
    }

    if (shaderAvailableAttribFlg & ShaderAttrib_EmMat)
        ptclAttributeBuffer->emtMat = *ptcl->pMatrixRT;
}

} } // namespace nw::eft
