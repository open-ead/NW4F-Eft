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

#define EMIT_FUNCTION_START()                                                 \
    const SimpleEmitterData* data = emitter->data;                            \
    const EmitterSet* emitterSet = emitter->emitterSet;                       \
                                                                              \
    f32 emissionRate = emitter->anim[0] * emitter->controller->_0;            \
    emitter->emitLostRate += emissionRate;                                    \
    s32 counter = (s32)emitter->counter;                                      \
    if (counter == 0 && emitter->emitLostRate < 1.0f && emissionRate != 0.0f) \
        emitter->emitLostRate = 1.0f;                                         \
                                                                              \
    s32 numEmit = (s32)floorf(emitter->emitLostRate);                         \
    if (data->_289 != 0) numEmit = 1;                                         \
    emitter->emitLostRate -= (f32)numEmit;                                    \
    if (numEmit == 0)                                                         \
        return NULL;                                                          \
                                                                              \
    f32 velocityMag = emitter->anim[15] * emitter->emitterSet->_244;

#define EMIT_LOOP_START()                                                   \
    PtclInstance* ptclFirst = NULL;                                         \
    for (s32 i = 0; i < numEmit; i++)                                       \
    {                                                                       \
        PtclInstance* ptcl = mSys->AllocPtcl(emitter->calc->GetPtclType()); \
        if (ptclFirst == NULL)                                              \
            ptclFirst = ptcl;                                               \
        if (ptcl == NULL)                                                   \
            break;                                                          \
                                                                            \
        ptcl->data = data;                                                  \
        ptcl->stripe = NULL;

#define EMIT_LOOP_FUNCTION_END()                                                            \
        if (data->_408 != 0.0f)                                                             \
        {                                                                                   \
            math::VEC3 posXZ = (math::VEC3){ ptcl->pos.x, 0.0f, ptcl->pos.z };              \
            if (posXZ.MagnitudeSquare() <= FLT_MIN) /* FLT_MIN = 1.1754943508222875E-38f */ \
            {                                                                               \
                posXZ.x = emitter->random.GetF32Range(-1.0f, 1.0f);                         \
                posXZ.z = emitter->random.GetF32Range(-1.0f, 1.0f);                         \
            }                                                                               \
                                                                                            \
            if (posXZ.MagnitudeSquare() == 0.0f)                                            \
                posXZ = (math::VEC3){ 0.0f, 0.0f, 0.0f };                                   \
            else                                                                            \
                posXZ.Normalize();                                                          \
                                                                                            \
            math::VEC3::Scale(&posXZ, &posXZ, data->_408);                                  \
            math::VEC3::Add(&ptcl->velocity, &ptcl->velocity, &posXZ);                      \
        }                                                                                   \
                                                                                            \
        EmitCommon(emitter, ptcl);                                                          \
    }                                                                                       \
                                                                                            \
    emitter->isEmitted = true;                                                              \
    return ptclFirst;

PtclInstance* EmitterCalc::CalcEmitPoint(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()
    (void)emitterSet; // Suppress unused warning

    EMIT_LOOP_START()

        ptcl->pos = math::VEC3::Zero();
        ptcl->velocity = emitter->random.GetNormalizedVec3() * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitCircle(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[23];

    f32 arcLength = math::Idx2Rad(data->arcLength);
    f32 arcStartAngle = math::Idx2Rad(data->arcStartAngle);
    if (data->_287 != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI;

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        ptcl->pos.x = sin_val * scaleX;
        ptcl->pos.y = 0.0f;
        ptcl->pos.z = cos_val * scaleZ;

        ptcl->velocity.x = sin_val * velocityMag;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cos_val * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitCircleSameDivide(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[23];

    u32 arcLengthUnit;
    u32 arcStartAngle = data->arcStartAngle;
    if (data->_287 != 0)
        arcStartAngle = emitter->random.GetU32();

    if (numEmit <= 1.0f) // Comparison with 1.0f instead of 1... mistake?
        arcLengthUnit = 0;

    else
    {
        if (data->arcLength == 0xFFFFFFFF)
            arcLengthUnit = 0xFFFFFFFF / numEmit;
        else
            arcLengthUnit = data->arcLength / (numEmit - 1);
    }

    u32 angle = arcStartAngle;

    EMIT_LOOP_START()

        f32 sin_val, cos_val;
        math::SinCosIdx(&sin_val, &cos_val, angle);

        ptcl->pos.x = sin_val * scaleX;
        ptcl->pos.y = 0.0f;
        ptcl->pos.z = cos_val * scaleZ;

        ptcl->velocity.x = sin_val * velocityMag;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cos_val * velocityMag;

        angle += arcLengthUnit;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitFillCircle(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[23];

    f32 arcLength = math::Idx2Rad(data->arcLength);
    f32 arcStartAngle = math::Idx2Rad(data->arcStartAngle);
    if (data->_287 != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI;

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        f32 v0 = emitter->random.GetF32();
        f32 v1 = 1.0f - data->_364;

        f32 a = v0 + v1 * v1 * (1.0f - v0);
        if (a <= 0.0f)
            a = 0.0f;
        else
            a = sqrtf(a);

        ptcl->pos.x = (a * sin_val) * scaleX;
        ptcl->pos.y = 0.0f;
        ptcl->pos.z = (a * cos_val) * scaleZ;

        ptcl->velocity.x = (a * sin_val) * velocityMag;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = (a * cos_val) * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitCylinder(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    f32 arcLength = math::Idx2Rad(data->arcLength);
    f32 arcStartAngle = math::Idx2Rad(data->arcStartAngle);
    if (data->_287 != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI;

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        ptcl->pos.x = sin_val * scaleX;
        ptcl->pos.y = emitter->random.GetF32Range(-1.0f, 1.0f) * scaleY;
        ptcl->pos.z = cos_val * scaleZ;

        ptcl->velocity.x = sin_val * velocityMag;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cos_val * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitFillCylinder(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    f32 arcLength = math::Idx2Rad(data->arcLength);
    f32 arcStartAngle = math::Idx2Rad(data->arcStartAngle);
    if (data->_287 != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI;

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        f32 v0 = emitter->random.GetF32();
        f32 v1 = 1.0f - data->_364;

        f32 a = v0 + v1 * v1 * (1.0f - v0);
        if (a <= 0.0f)
            a = 0.0f;
        else
            a = sqrtf(a);

        ptcl->pos.x = (sin_val * a) * scaleX;
        ptcl->pos.y = emitter->random.GetF32Range(-1.0f, 1.0f) * scaleY;
        ptcl->pos.z = (cos_val * a) * scaleZ;

        ptcl->velocity.x = sin_val * velocityMag;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cos_val * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitLine(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[22];
    f32 center = data->_378 * scaleZ;

    EMIT_LOOP_START()

        ptcl->pos.x = 0.0f;
        ptcl->pos.y = 0.0f;
        ptcl->pos.z = emitter->random.GetF32() * scaleZ - (scaleZ + center) / 2.0f;

        ptcl->velocity.x = 0.0f;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitLineSameDivide(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[22];
    f32 center = data->_378 * scaleZ;

    f32 pos;
    f32 lengthUnit;

    if (numEmit == 1)
    {
        pos = 0.5f;
        lengthUnit = 0.0f;
    }
    else
    {
        pos = 0.0f;
        lengthUnit = 1.0f / (f32)(numEmit - 1);
    }

    EMIT_LOOP_START()

        ptcl->pos.x = 0.0f;
        ptcl->pos.y = 0.0f;
        ptcl->pos.z = pos * scaleZ - (scaleZ + center) / 2.0f;

        ptcl->velocity.x = 0.0f;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = velocityMag;

        pos += lengthUnit;

    EMIT_LOOP_FUNCTION_END()
}

} } // namespace nw::eft
