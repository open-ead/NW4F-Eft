#include <eft_Animation.h>
#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

EmitterCalc::AnimFunction EmitterCalc::mAnimFunctionsSclae[] = {
    EmitterCalc::CalcPtclAnimation,
    EmitterCalc::ptclAnim_Scale_4k3v,
    EmitterCalc::ptclAnim_Scale_8key,
};

EmitterCalc::AnimFunction EmitterCalc::mAnimFunctionsColor0[] = {
    EmitterCalc::CalcPtclAnimation,
    EmitterCalc::ptclAnim_Color0_4k3v,
    EmitterCalc::ptclAnim_Color0_8key,
};

EmitterCalc::AnimFunction EmitterCalc::mAnimFunctionsColor1[] = {
    EmitterCalc::CalcPtclAnimation,
    EmitterCalc::ptclAnim_Color1_4k3v,
    EmitterCalc::ptclAnim_Color1_8key,
};

EmitterCalc::AnimFunction EmitterCalc::mAnimFunctionsAlpha0[] = {
    EmitterCalc::CalcPtclAnimation,
    EmitterCalc::ptclAnim_Alpha0_4k3v,
    EmitterCalc::ptclAnim_Alpha0_8key,
};

EmitterCalc::AnimFunction EmitterCalc::mAnimFunctionsAlpha1[] = {
    EmitterCalc::CalcPtclAnimation,
    EmitterCalc::ptclAnim_Alpha1_4k3v,
    EmitterCalc::ptclAnim_Alpha1_8key,
};

// No division-by-zero checks whatsoever...

static inline void ColorAnim_4k3v(u32 colorIdx, PtclInstance* ptcl, const SimpleEmitterData* data)
{
    s32 period = ptcl->lifespan / data->colorNumRepetition[colorIdx];
    if (period == 0)
        period = ptcl->lifespan;

    s32 counter = ptcl->counterS32;
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

static inline void CalcParitcleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    const SimpleEmitterData* data = ptcl->data;

    f32 emissionSpeedInv = 1.0f - emissionSpeed;

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

    ptcl->pos += ptcl->velocity * (ptcl->randomF32 * emissionSpeed);

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_AirResist)
        ptcl->velocity *= data->airResist + (1.0f - data->airResist) * emissionSpeedInv;

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Gravity)
    {
        math::VEC3 gravity = data->gravity * (emissionSpeed * emitter->anim[25]);

        if (data->transformGravity != 0)
            ptcl->velocity += *math::VEC3::MultMTX(&gravity, &gravity, matrixRT);

        else
            ptcl->velocity += gravity;
    }

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Rotate)
        ptcl->rotation += ptcl->angularVelocity * emissionSpeed;

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_RotInertia)
        math::VEC3::Scale(&ptcl->angularVelocity, &ptcl->angularVelocity, data->rotInertia + (1.0f - data->rotInertia) * emissionSpeedInv);

    if (data->alphaAnimType[0] != AnimationType_None)
        EmitterCalc::mAnimFunctionsAlpha0[data->alphaAnimType[0]](emitter, ptcl, emissionSpeed);

    if (data->alphaAnimType[1] != AnimationType_None)
        EmitterCalc::mAnimFunctionsAlpha1[data->alphaAnimType[1]](emitter, ptcl, emissionSpeed);

    if (data->scaleAnimType != AnimationType_None)
        EmitterCalc::mAnimFunctionsSclae[data->scaleAnimType](emitter, ptcl, emissionSpeed);

    if (data->colorAnimType[0] != AnimationType_None)
        EmitterCalc::mAnimFunctionsColor0[data->colorAnimType[0]](emitter, ptcl, emissionSpeed);

    if (data->colorAnimType[1] != AnimationType_None)
        EmitterCalc::mAnimFunctionsColor1[data->colorAnimType[1]](emitter, ptcl, emissionSpeed);

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_Unk)
        ptcl->_13C = (ptcl->velocity.Magnitude() + ptcl->posDiff.Magnitude() * ptcl->randomF32) * (ptcl->data->_7F4 * ptcl->randomF32);
}

void EmitterCalc::CalcSimpleParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    math::VEC3 posBefore = ptcl->pos;
    math::MTX34* matrixSRT = ptcl->pMatrixSRT;

    CalcParitcleBehavior(emitter, ptcl, emissionSpeed);

    if (emitter->particleBehaviorFlg & ParticleBehaviorFlag_WldPosDf && emissionSpeed != 0.0f)
    {
        math::VEC3 posDiff = ptcl->pos - posBefore;
        if (fabsf(posDiff.x) > 0.0001 || fabsf(posDiff.y) > 0.0001 || fabsf(posDiff.z) > 0.0001)
            ptcl->posDiff += posDiff - ptcl->posDiff;

        math::MTX34::MultVecSR(&ptcl->worldPosDiff, matrixSRT, &ptcl->posDiff);
    }

    math::MTX34::MultVec(&ptcl->worldPos, matrixSRT, &ptcl->pos);

    ptcl->counter += emissionSpeed;
}

void EmitterCalc::CalcComplexParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    const ComplexEmitterData* data = ptcl->cdata;

    math::VEC3 posBefore = ptcl->pos;
    math::MTX34* matrixSRT = ptcl->pMatrixSRT;

    CalcParitcleBehavior(emitter, ptcl, emissionSpeed);

    if (data->fluctuationFlags & 1)
        CalcFluctuation(emitter, ptcl);

    if (data->fieldFlags != 0)
    {
        const void* fieldData = reinterpret_cast<const void*>((u32)data + data->fieldDataOffs);

        if (data->fieldFlags & 0x01) fieldData = _ptclField_Random     (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x02) fieldData = _ptclField_Magnet     (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x04) fieldData = _ptclField_Spin       (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x08) fieldData = _ptclField_Collision  (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x10) fieldData = _ptclField_Convergence(emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x20) fieldData = _ptclField_PosAdd     (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x40) fieldData = _ptclField_CurlNoise  (emitter, ptcl, fieldData, emissionSpeed);
    }

    if (emissionSpeed != 0.0f && emitter->particleBehaviorFlg & ParticleBehaviorFlag_WldPosDf)
    {
        math::VEC3 posDiff = ptcl->pos - posBefore;
        if (fabsf(posDiff.x) > 0.0001 || fabsf(posDiff.y) > 0.0001 || fabsf(posDiff.z) > 0.0001)
            ptcl->posDiff += posDiff - ptcl->posDiff;

        math::MTX34::MultVecSR(&ptcl->worldPosDiff, matrixSRT, &ptcl->posDiff);
    }

    math::MTX34::MultVec(&ptcl->worldPos, matrixSRT, &ptcl->pos);

    ptcl->counter += emissionSpeed;
}

void EmitterCalc::CalcChildParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);
    const ChildData* childData = reinterpret_cast<const ChildData*>(data + 1);

    math::VEC3 posBefore = ptcl->pos;

    math::MTX34* matrixRT = ptcl->pMatrixRT;
    math::MTX34* matrixSRT = ptcl->pMatrixSRT;

    f32 emissionSpeedInv = 1.0f - emissionSpeed;

    ptcl->pos += ptcl->velocity * ptcl->randomF32 * emissionSpeed;

    if (emitter->childParticleBehaviorFlg & ParticleBehaviorFlag_AirResist)
        ptcl->velocity *= childData->airResist + (1.0f - data->airResist) * emissionSpeedInv; // No idea why it uses data->airResist, mistake?

    if (emitter->childParticleBehaviorFlg & ParticleBehaviorFlag_Gravity)
    {
        math::VEC3 gravity = childData->gravity * (emissionSpeed * emitter->anim[26]);

        if (data->childFlags & 0x100)
            ptcl->velocity += *math::VEC3::MultMTX(&gravity, &gravity, matrixRT);

        else
            ptcl->velocity += gravity;
    }

    AnimationType alphaAnimType0 = static_cast<AnimationType>(childData->ptclAlphaSrc[0]);
    AnimationType alphaAnimType1 = static_cast<AnimationType>(childData->ptclAlphaSrc[1]);

    if (alphaAnimType0 != AnimationType_None)
        EmitterCalc::mAnimFunctionsAlpha0[alphaAnimType0](emitter, ptcl, emissionSpeed);

    if (alphaAnimType1 != AnimationType_None)
        EmitterCalc::mAnimFunctionsAlpha1[alphaAnimType1](emitter, ptcl, emissionSpeed);

    if (emitter->childParticleBehaviorFlg & ParticleBehaviorFlag_ScaleAnim)
    {
        if (ptcl->counter >= childData->scaleAnimTime1)
        {
            ptcl->scale.x += ptcl->scaleAnim->startDiff.x * emissionSpeed;
            ptcl->scale.y += ptcl->scaleAnim->startDiff.y * emissionSpeed;
        }
    }

    if (emitter->childParticleBehaviorFlg & ParticleBehaviorFlag_Rotate)
        ptcl->rotation += ptcl->angularVelocity * emissionSpeed;

    if (emitter->childParticleBehaviorFlg & ParticleBehaviorFlag_RotInertia)
        math::VEC3::Scale(&ptcl->angularVelocity, &ptcl->angularVelocity, childData->rotInertia + (1.0f - childData->rotInertia) * emissionSpeedInv);

    if (data->childFlags & 0x800)
    {
        const void* fieldData = reinterpret_cast<const void*>((u32)data + data->fieldDataOffs);

        if (data->fieldFlags & 0x01) fieldData = _ptclField_Random     (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x02) fieldData = _ptclField_Magnet     (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x04) fieldData = _ptclField_Spin       (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x08) fieldData = _ptclField_Collision  (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x10) fieldData = _ptclField_Convergence(emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x20) fieldData = _ptclField_PosAdd     (emitter, ptcl, fieldData, emissionSpeed);
        if (data->fieldFlags & 0x40) fieldData = _ptclField_CurlNoise  (emitter, ptcl, fieldData, emissionSpeed);
    }

    math::MTX34::MultVec(&ptcl->worldPos, &ptcl->matrixSRT, &ptcl->pos);

    if (emitter->childParticleBehaviorFlg & ParticleBehaviorFlag_WldPosDf && emissionSpeed != 0.0f)
    {
        math::VEC3 posDiff = ptcl->pos - posBefore;
        if (fabsf(posDiff.x) > 0.0001 || fabsf(posDiff.y) > 0.0001 || fabsf(posDiff.z) > 0.0001)
            ptcl->posDiff += posDiff - ptcl->posDiff;

        math::MTX34::MultVecSR(&ptcl->worldPosDiff, matrixSRT, &ptcl->posDiff);
    }

    ptcl->counter += emissionSpeed;
}

void EmitterCalc::MakeParticleAttributeBuffer(PtclAttributeBuffer* ptclAttributeBuffer, PtclInstance* ptcl, u32 shaderAvailableAttribFlg)
{
    const EmitterSet* emitterSet = ptcl->emitter->emitterSet;

    ptclAttributeBuffer->wldPos.xyz() = ptcl->worldPos;
    ptclAttributeBuffer->wldPos.w = ptcl->counter;

    if (shaderAvailableAttribFlg & ShaderAttrib_Scl)
    {
        ptclAttributeBuffer->scl.x = ptcl->scale.x * emitterSet->ptclEffectiveScale.x * ptcl->fluctuationScale.x;
        ptclAttributeBuffer->scl.y = ptcl->scale.y * emitterSet->ptclEffectiveScale.y * ptcl->fluctuationScale.y + ptcl->_13C;
        ptclAttributeBuffer->scl.z = 0.0f;
        ptclAttributeBuffer->scl.w = 0.0f;
    }

    if (shaderAvailableAttribFlg & ShaderAttrib_Color0)
    {
        ptclAttributeBuffer->color0.xyz() = ptcl->color0.rgb();
        ptclAttributeBuffer->color0.w = ptcl->alpha0 * ptcl->fluctuationAlpha;
    }

    ptclAttributeBuffer->random.xyz() = ptcl->randomVec4.xyz();
    ptclAttributeBuffer->random.w = (f32)ptcl->lifespan;

    if (shaderAvailableAttribFlg & ShaderAttrib_Vec)
    {
        ptclAttributeBuffer->vec.xyz() = ptcl->velocity;
        ptclAttributeBuffer->vec.w = ptcl->randomF32;
    }

    if (shaderAvailableAttribFlg & ShaderAttrib_Color1)
    {
        ptclAttributeBuffer->color1.xyz() = ptcl->color1.rgb();
        ptclAttributeBuffer->color1.w = ptcl->alpha1 * ptcl->fluctuationAlpha;
    }

    if (shaderAvailableAttribFlg & ShaderAttrib_Rot)
    {
        ptclAttributeBuffer->rot.xyz() = ptcl->rotation;
        ptclAttributeBuffer->rot.w = 0.0f;
    }

    if (shaderAvailableAttribFlg & ShaderAttrib_WldPosDf)
    {
        ptclAttributeBuffer->wldPosDf.xyz() = ptcl->worldPosDiff;
        ptclAttributeBuffer->wldPosDf.w = 1.0f;
    }

    if (shaderAvailableAttribFlg & ShaderAttrib_EmMat)
    {
        ptclAttributeBuffer->emtMat = *ptcl->pMatrixRT;

        if (ptcl->velocity.x == ptcl->pMatrixRT->m[0][1] && ptcl->velocity.z == ptcl->pMatrixRT->m[2][1])
            ptclAttributeBuffer->emtMat.m[2][1] += 0.0001f;
    }
}

void EmitterCalc::ptclAnim_Scale_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    KeyFrameAnim* scaleXAnim = emitter->ptclAnimArray[46 - (27 + 1)];
    KeyFrameAnim* scaleYAnim = emitter->ptclAnimArray[47 - (27 + 1)];

    f32 time = _calcParticleAnimTime(scaleXAnim, ptcl, 46);
    u32 index = CalcAnimKeyFrameIndex(scaleXAnim, time);

    ptcl->scale.x = CalcAnimKeyFrameSimple(scaleXAnim, time, index) * ptcl->data->ptclEmitScale.x * emitter->anim[17];
    ptcl->scale.y = CalcAnimKeyFrameSimple(scaleYAnim, time, index) * ptcl->data->ptclEmitScale.y * emitter->anim[18];

    if (emitter->data->ptclScaleRandom.x == emitter->data->ptclScaleRandom.y)
        ptcl->scale *= 1.0f - emitter->data->ptclScaleRandom.x * ptcl->randomVec4.x;

    else
    {
        ptcl->scale.x *= 1.0f - emitter->data->ptclScaleRandom.x * ptcl->randomVec4.x;
        ptcl->scale.y *= 1.0f - emitter->data->ptclScaleRandom.y * ptcl->randomVec4.y;
    }
}

void EmitterCalc::ptclAnim_Scale_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    if (ptcl->counter <= ptcl->scaleAnim->time2)
    {
        ptcl->scale.x += ptcl->scaleAnim->startDiff.x * emissionSpeed;
        ptcl->scale.y += ptcl->scaleAnim->startDiff.y * emissionSpeed;
    }
    else if (ptcl->counter > ptcl->scaleAnim->time3)
    {
        ptcl->scale.x += ptcl->scaleAnim->endDiff.x * emissionSpeed;
        ptcl->scale.y += ptcl->scaleAnim->endDiff.y * emissionSpeed;
    }
}

void EmitterCalc::ptclAnim_Color0_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    KeyFrameAnim* colorRAnim = emitter->ptclAnimArray[38 - (27 + 1)];
    KeyFrameAnim* colorGAnim = emitter->ptclAnimArray[39 - (27 + 1)];
    KeyFrameAnim* colorBAnim = emitter->ptclAnimArray[40 - (27 + 1)];

    f32 time = _calcParticleAnimTime(colorRAnim, ptcl, 38);
    u32 index = CalcAnimKeyFrameIndex(colorRAnim, time);

    ptcl->color[0].r = CalcAnimKeyFrameSimple(colorRAnim, time, index);
    ptcl->color[0].g = CalcAnimKeyFrameSimple(colorGAnim, time, index);
    ptcl->color[0].b = CalcAnimKeyFrameSimple(colorBAnim, time, index);
}

void EmitterCalc::ptclAnim_Color1_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    KeyFrameAnim* colorRAnim = emitter->ptclAnimArray[41 - (27 + 1)];
    KeyFrameAnim* colorGAnim = emitter->ptclAnimArray[42 - (27 + 1)];
    KeyFrameAnim* colorBAnim = emitter->ptclAnimArray[43 - (27 + 1)];

    f32 time = _calcParticleAnimTime(colorRAnim, ptcl, 41);
    u32 index = CalcAnimKeyFrameIndex(colorRAnim, time);

    ptcl->color[1].r = CalcAnimKeyFrameSimple(colorRAnim, time, index);
    ptcl->color[1].g = CalcAnimKeyFrameSimple(colorGAnim, time, index);
    ptcl->color[1].b = CalcAnimKeyFrameSimple(colorBAnim, time, index);
}

void EmitterCalc::ptclAnim_Color0_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    ColorAnim_4k3v(0, ptcl, ptcl->data);
}

void EmitterCalc::ptclAnim_Color1_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    ColorAnim_4k3v(1, ptcl, ptcl->data);
}

void EmitterCalc::ptclAnim_Alpha0_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    KeyFrameAnim* alphaAnim = emitter->ptclAnimArray[44 - (27 + 1)];

    f32 time = _calcParticleAnimTime(alphaAnim, ptcl, 44);
    u32 index = CalcAnimKeyFrameIndex(alphaAnim, time);

    ptcl->alpha[0] = CalcAnimKeyFrameSimple(alphaAnim, time, index);
}

void EmitterCalc::ptclAnim_Alpha0_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    if (ptcl->counter <= ptcl->alphaAnim[0]->time2)
        ptcl->alpha[0] += ptcl->alphaAnim[0]->startDiff * emissionSpeed;

    else if (ptcl->counter > ptcl->alphaAnim[0]->time3)
        ptcl->alpha[0] += ptcl->alphaAnim[0]->endDiff * emissionSpeed;
}

void EmitterCalc::ptclAnim_Alpha1_8key(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    KeyFrameAnim* alphaAnim = emitter->ptclAnimArray[45 - (27 + 1)];

    f32 time = _calcParticleAnimTime(alphaAnim, ptcl, 45);
    u32 index = CalcAnimKeyFrameIndex(alphaAnim, time);

    ptcl->alpha[1] = CalcAnimKeyFrameSimple(alphaAnim, time, index);
}

void EmitterCalc::ptclAnim_Alpha1_4k3v(EmitterInstance* emitter, PtclInstance* ptcl, f32 emissionSpeed)
{
    if (ptcl->counter <= ptcl->alphaAnim[1]->time2)
        ptcl->alpha[1] += ptcl->alphaAnim[1]->startDiff * emissionSpeed;

    else if (ptcl->counter > ptcl->alphaAnim[1]->time3)
        ptcl->alpha[1] += ptcl->alphaAnim[1]->endDiff * emissionSpeed;
}

} } // namespace nw::eft
