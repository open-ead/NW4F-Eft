#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_System.h>

#include <cfloat>

namespace nw { namespace eft {

EmitterCalc::EmitFunction EmitterCalc::mEmitFunctions[] = {
    EmitterCalc::CalcEmitPoint,
    EmitterCalc::CalcEmitCircle,
    EmitterCalc::CalcEmitCircleSameDivide,
    EmitterCalc::CalcEmitFillCircle,
    EmitterCalc::CalcEmitSphere,
    EmitterCalc::CalcEmitSphereSameDivide,
    EmitterCalc::CalcEmitSphereSameDivide64,
    EmitterCalc::CalcEmitFillSphere,
    EmitterCalc::CalcEmitCylinder,
    EmitterCalc::CalcEmitFillCylinder,
    EmitterCalc::CalcEmitBox,
    EmitterCalc::CalcEmitFillBox,
    EmitterCalc::CalcEmitLine,
    EmitterCalc::CalcEmitLineSameDivide,
    EmitterCalc::CalcEmitRectangle,
};

PtclInstance* EmitterCalc::CalcEmitPoint(EmitterInstance* emitter)
{
    const SimpleEmitterData* data = emitter->data;

    f32 emissionRate = emitter->anim[0] * emitter->controller->_0;
    emitter->emitLostRate += emissionRate;
    s32 counter = (s32)emitter->counter;
    if (counter == 0 && emitter->emitLostRate < 1.0f && emissionRate != 0.0f)
        emitter->emitLostRate = 1.0f;

    s32 numEmit = (s32)floorf(emitter->emitLostRate);
    if (data->_289 != 0) numEmit = 1;
    emitter->emitLostRate -= (f32)numEmit;
    if (numEmit == 0)
        return NULL;

    f32 velocityMag = emitter->anim[15] * emitter->emitterSet->_244;

    PtclInstance* ptclFirst = NULL;
    for (s32 i = 0; i < numEmit; i++)
    {
        PtclInstance* ptcl = mSys->AllocPtcl(emitter->calc->GetPtclType());
        if (ptclFirst == NULL)
            ptclFirst = ptcl;
        if (ptcl == NULL)
            break;

        ptcl->data = data;
        ptcl->stripe = NULL;

        ptcl->pos = math::VEC3::Zero();
        ptcl->velocity = emitter->random.GetNormalizedVec3() * velocityMag;

        if (data->_408 != 0.0f)
        {
            math::VEC3 posXZ = (math::VEC3){ ptcl->pos.x, 0.0f, ptcl->pos.z };
            if (posXZ.MagnitudeSquare() <= FLT_MIN) // FLT_MIN = 1.1754943508222875E-38f
            {
                posXZ.x = emitter->random.GetF32Range(-1.0f, 1.0f);
                posXZ.z = emitter->random.GetF32Range(-1.0f, 1.0f);
            }

            if (posXZ.MagnitudeSquare() == 0.0f)
                posXZ = (math::VEC3){ 0.0f, 0.0f, 0.0f };
            else
                posXZ.Normalize();

            math::VEC3::Scale(&posXZ, &posXZ, data->_408);
            math::VEC3::Add(&ptcl->velocity, &ptcl->velocity, &posXZ);
        }

        EmitCommon(emitter, ptcl);
    }

    emitter->isEmitted = true;
    return ptclFirst;
}

PtclInstance* EmitterCalc::CalcEmitCircle(EmitterInstance* emitter)
{
    const SimpleEmitterData* data = emitter->data;
    const EmitterSet* emitterSet = emitter->emitterSet;

    f32 emissionRate = emitter->anim[0] * emitter->controller->_0;
    emitter->emitLostRate += emissionRate;
    s32 counter = (s32)emitter->counter;
    if (counter == 0 && emitter->emitLostRate < 1.0f && emissionRate != 0.0f)
        emitter->emitLostRate = 1.0f;

    s32 numEmit = (s32)floorf(emitter->emitLostRate);
    if (data->_289 != 0) numEmit = 1;
    emitter->emitLostRate -= (f32)numEmit;
    if (numEmit == 0)
        return NULL;

    f32 velocityMag = emitter->anim[15] * emitter->emitterSet->_244;

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[23];

    f32 arcLength = math::Idx2Rad(data->arcLength);
    f32 arcStartAngle = math::Idx2Rad(data->arcStartAngle);
    if (data->_287 != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI;

    PtclInstance* ptclFirst = NULL;
    for (s32 i = 0; i < numEmit; i++)
    {
        PtclInstance* ptcl = mSys->AllocPtcl(emitter->calc->GetPtclType());
        if (ptclFirst == NULL)
            ptclFirst = ptcl;
        if (ptcl == NULL)
            break;

        ptcl->data = data;
        ptcl->stripe = NULL;

        f32 sin_val, cos_val, angle = emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        ptcl->pos.x = sin_val * scaleX;
        ptcl->pos.y = 0.0f;
        ptcl->pos.z = cos_val * scaleZ;

        ptcl->velocity.x = sin_val * velocityMag;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cos_val * velocityMag;

        if (data->_408 != 0.0f)
        {
            math::VEC3 posXZ = (math::VEC3){ ptcl->pos.x, 0.0f, ptcl->pos.z };
            if (posXZ.MagnitudeSquare() <= FLT_MIN) // FLT_MIN = 1.1754943508222875E-38f
            {
                posXZ.x = emitter->random.GetF32Range(-1.0f, 1.0f);
                posXZ.z = emitter->random.GetF32Range(-1.0f, 1.0f);
            }

            if (posXZ.MagnitudeSquare() == 0.0f)
                posXZ = (math::VEC3){ 0.0f, 0.0f, 0.0f };
            else
                posXZ.Normalize();

            math::VEC3::Scale(&posXZ, &posXZ, data->_408);
            math::VEC3::Add(&ptcl->velocity, &ptcl->velocity, &posXZ);
        }

        EmitCommon(emitter, ptcl);
    }

    emitter->isEmitted = true;
    return ptclFirst;
}

} } // namespace nw::eft
