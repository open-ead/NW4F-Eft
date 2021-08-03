#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_Primitive.h>
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
    EmitterCalc::CalcEmitPrimitive,
};

#include "eft_EmitterVolume.hpp"

void EmitterCalc::CalcEmitPoint(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()
    (void)emitterSet; // Suppress unused warning

    for (s32 i = 0; i < numEmit; i++)
    {
        PtclInstance* ptcl = mSys->AllocPtcl(emitter);
        if (ptcl == NULL)
            return;

        ptcl->pos = math::VEC3::Zero();
        ptcl->velocity = emitter->random.GetNormalizedVec3() * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitCircle(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[23];

    f32 arcLength = data->arcLength;
    f32 arcStartAngle = data->arcStartAngle;
    if (data->arcStartAngleRandom != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI * 2.0f;

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

void EmitterCalc::CalcEmitCircleSameDivide(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[23];

    f32 arcLengthUnit;
    f32 arcStartAngle = data->arcStartAngle;
    if (data->arcStartAngleRandom != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI * 2.0f;

    if (numEmit <= 1.0f) // Comparison with 1.0f instead of 1... mistake?
        arcLengthUnit = 0;

    else
    {
        if (data->arcLength == math::F_PI * 2.0f)
            arcLengthUnit = data->arcLength / numEmit;
        else
            arcLengthUnit = data->arcLength / (numEmit - 1);
    }

    f32 angle = arcStartAngle;

    EMIT_LOOP_START()

        f32 sin_val, cos_val, rndAngle = angle;
        if (data->arcRandom != 0.0f)
            rndAngle += emitter->random.GetF32Range(-1.0f, 1.0f) * data->arcRandom / 180.0f * math::F_PI;
        math::SinCosRad(&sin_val, &cos_val, rndAngle);

        ptcl->pos.x = sin_val * scaleX;
        ptcl->pos.y = 0.0f;
        ptcl->pos.z = cos_val * scaleZ;

        ptcl->velocity.x = sin_val * velocityMag;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = cos_val * velocityMag;

        angle += arcLengthUnit;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitFillCircle(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[23];

    f32 arcLength = data->arcLength;
    f32 arcStartAngle = data->arcStartAngle;
    if (data->arcStartAngleRandom != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI * 2.0f;

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        f32 v0 = emitter->random.GetF32();
        f32 v1 = 1.0f - data->volumeFillRatio;

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

void EmitterCalc::CalcEmitSphere(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->emitterVolumeScale.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[24];

    f32 arcLength, arcStartAngle;

    if (data->sphereUseLatitude != 0)
    {
        arcLength = 0.0f;
        arcStartAngle = 0.0f;
    }
    else
    {
        arcLength = data->arcLength;
        arcStartAngle = (data->arcStartAngleRandom != 0) ? emitter->random.GetF32() * math::F_PI * 2.0f
                                                         : data->arcStartAngle;
    }

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = (data->sphereUseLatitude != 0) ? emitter->random.GetF32() * math::F_PI * 2.0f
                                                                     : emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        f32 y = (data->sphereUseLatitude != 0) ? 1.0f - (1.0f - math::CosRad(data->sphereLatitude)) * emitter->random.GetF32()
                                               : emitter->random.GetF32Range(-1.0f, 1.0f);

        f32 a = 1.0f - y * y;
        if (a <= 0.0f)
            a = 0.0f;
        else
            a = sqrtf(a);

        math::VEC3 normalizedVel = (math::VEC3){ a * sin_val, y, a * cos_val };

        if (data->sphereUseLatitude != 0 && (data->sphereLatitudeDir.x != 0.0f || data->sphereLatitudeDir.y != 1.0f || data->sphereLatitudeDir.z != 0.0f))
        {
            math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
            math::MTX34 mtx;
            math::MTX34::MakeVectorRotation(&mtx, &base, &data->sphereLatitudeDir);

            math::MTX34::PSMultVec(&normalizedVel, &mtx, &normalizedVel);
        }

        ptcl->pos.x = normalizedVel.x * scaleX;
        ptcl->pos.y = normalizedVel.y * scaleY;
        ptcl->pos.z = normalizedVel.z * scaleZ;

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = normalizedVel.y * velocityMag;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitSphereSameDivide(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->emitterVolumeScale.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[24];

    const math::VEC3* table = gSameDivideSphereTbl[data->sphereDivTblIdx];

    for (s32 i = 0; i < numEmit; i++)
    {
        math::VEC3 normalizedVel = *table++;

        if (data->sphereUseLatitude != 0)
        {
            if (!(math::F_PI - 0.0001f < data->sphereLatitude || math::CosRad(data->sphereLatitude) < normalizedVel.y))
                continue;
        }

        PtclStripe* stripe = NULL;
        if (data->vertexTransformMode == VertexTransformMode_Stripe)
        {
            stripe = mSys->AllocAndConnectStripe(emitter);
            if (stripe == NULL)
                break;
        }

        PtclInstance* ptcl = mSys->AllocPtcl();
        if (ptcl == NULL)
            continue;

        ptcl->type = emitter->calc->GetPtclType();
        ptcl->data = emitter->data;
        ptcl->complexParam->stripe = stripe;
        if (stripe != NULL)
            stripe->particle = ptcl;

        if (data->sphereUseLatitude != 0 && (data->sphereLatitudeDir.x != 0.0f || data->sphereLatitudeDir.y != 1.0f || data->sphereLatitudeDir.z != 0.0f))
        {
            math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
            math::MTX34 mtx;
            math::MTX34::MakeVectorRotation(&mtx, &base, &data->sphereLatitudeDir);

            math::MTX34::PSMultVec(&normalizedVel, &mtx, &normalizedVel);
        }

        ptcl->pos.x = normalizedVel.x * scaleX;
        ptcl->pos.y = normalizedVel.y * scaleY;
        ptcl->pos.z = normalizedVel.z * scaleZ;

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = normalizedVel.y * velocityMag;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitSphereSameDivide64(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->emitterVolumeScale.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[24];

    const math::VEC3* table = gSameDivideSphere64Tbl[data->sphereDivTblIdx + 2]; // What is the point of the first 2 tables?

    for (s32 i = 0; i < numEmit; i++)
    {
        math::VEC3 normalizedVel = *table++;

        if (data->sphereUseLatitude != 0)
        {
            if (!(math::F_PI - 0.0001f < data->sphereLatitude || math::CosRad(data->sphereLatitude) < normalizedVel.y))
                continue;
        }

        PtclStripe* stripe = NULL;
        if (data->vertexTransformMode == VertexTransformMode_Stripe)
        {
            stripe = mSys->AllocAndConnectStripe(emitter);
            if (stripe == NULL)
                break;
        }

        PtclInstance* ptcl = mSys->AllocPtcl();
        if (ptcl == NULL)
            continue;

        ptcl->type = emitter->calc->GetPtclType();
        ptcl->data = emitter->data;
        ptcl->complexParam->stripe = stripe;
        if (stripe != NULL)
            stripe->particle = ptcl;

        if (data->sphereUseLatitude != 0 && (data->sphereLatitudeDir.x != 0.0f || data->sphereLatitudeDir.y != 1.0f || data->sphereLatitudeDir.z != 0.0f))
        {
            math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
            math::MTX34 mtx;
            math::MTX34::MakeVectorRotation(&mtx, &base, &data->sphereLatitudeDir);

            math::MTX34::PSMultVec(&normalizedVel, &mtx, &normalizedVel);
        }

        ptcl->pos.x = normalizedVel.x * scaleX;
        ptcl->pos.y = normalizedVel.y * scaleY;
        ptcl->pos.z = normalizedVel.z * scaleZ;

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = normalizedVel.y * velocityMag;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitFillSphere(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->emitterVolumeScale.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[24];

    f32 arcLength, arcStartAngle;

    if (data->sphereUseLatitude != 0)
    {
        arcLength = 0.0f;
        arcStartAngle = 0.0f;
    }
    else
    {
        arcLength = data->arcLength;
        arcStartAngle = (data->arcStartAngleRandom != 0) ? emitter->random.GetF32() * math::F_PI * 2.0f
                                                         : data->arcStartAngle;
    }

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = (data->sphereUseLatitude != 0) ? emitter->random.GetF32() * math::F_PI * 2.0f
                                                                     : emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        f32 y = (data->sphereUseLatitude != 0) ? 1.0f - (1.0f - math::CosRad(data->sphereLatitude)) * emitter->random.GetF32()
                                               : emitter->random.GetF32Range(-1.0f, 1.0f);

        f32 a = 1.0f - y * y;
        if (a <= 0.0f)
            a = 0.0f;
        else
            a = sqrtf(a);

        f32 extent = emitter->random.GetF32();
        if (extent <= 0.0f)
            extent = 0.0f;
        else
            extent = sqrtf(extent);
        extent = extent * data->volumeFillRatio + 1.0f - data->volumeFillRatio;

        math::VEC3 normalizedVel = (math::VEC3){ a * sin_val, y, a * cos_val };

        if (data->sphereUseLatitude != 0 && (data->sphereLatitudeDir.x != 0.0f || data->sphereLatitudeDir.y != 1.0f || data->sphereLatitudeDir.z != 0.0f))
        {
            math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
            math::MTX34 mtx;
            math::MTX34::MakeVectorRotation(&mtx, &base, &data->sphereLatitudeDir);

            math::MTX34::PSMultVec(&normalizedVel, &mtx, &normalizedVel);
        }

        ptcl->pos.x = normalizedVel.x * scaleX * extent;
        ptcl->pos.y = normalizedVel.y * scaleY * extent;
        ptcl->pos.z = normalizedVel.z * scaleZ * extent;

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = normalizedVel.y * velocityMag;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitCylinder(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->emitterVolumeScale.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[24];

    f32 arcLength = data->arcLength;
    f32 arcStartAngle = data->arcStartAngle;
    if (data->arcStartAngleRandom != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI * 2.0f;

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

void EmitterCalc::CalcEmitFillCylinder(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->emitterVolumeScale.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[24];

    f32 arcLength = data->arcLength;
    f32 arcStartAngle = data->arcStartAngle;
    if (data->arcStartAngleRandom != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI * 2.0f;

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        f32 v0 = emitter->random.GetF32();
        f32 v1 = 1.0f - data->volumeFillRatio;

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

void EmitterCalc::CalcEmitBox(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->emitterVolumeScale.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[24];

    EMIT_LOOP_START()

        u32 v0 = emitter->random.GetU32();
        u32 v1 = emitter->random.GetU32();

        math::VEC3 rndVec = {
            .x = emitter->random.GetF32Range(-1.0f, 1.0f),
            .y = emitter->random.GetF32Range(-1.0f, 1.0f),
            .z = emitter->random.GetF32Range(-1.0f, 1.0f),
        };

        if (v0 < 0x55555555)
        {
            ptcl->pos.x = scaleX * rndVec.x;
            ptcl->pos.y = scaleY * rndVec.y;
            ptcl->pos.z = (v1 < 0x7fffffff) ? scaleZ : -scaleZ;
        }
        else if (v0 < 0xAAAAAAAA)
        {
            ptcl->pos.x = scaleX * rndVec.x;
            ptcl->pos.y = (v1 < 0x7fffffff) ? scaleY : -scaleY;
            ptcl->pos.z = scaleZ * rndVec.z;
        }
        else // if (v0 < 0xFFFFFFFF)
        {
            ptcl->pos.x = (v1 < 0x7fffffff) ? scaleX : -scaleX;
            ptcl->pos.y = scaleY * rndVec.y;
            ptcl->pos.z = scaleZ * rndVec.z;
        }

        math::VEC3 normalizedVel;
        if (ptcl->pos.x != 0.0f || ptcl->pos.y != 0.0f || ptcl->pos.z != 0.0f)
        {
            normalizedVel = ptcl->pos;
            normalizedVel.Normalize();
        }

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = normalizedVel.y * velocityMag;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitFillBox(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->emitterVolumeScale.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[24];

    EMIT_LOOP_START()

        f32 v0 = 1.0f - data->volumeFillRatio;
        if (v0 == 1.0f)
            v0 = 0.999f;

        if (v0 == 0.0f)
        {
            ptcl->pos.x = scaleX * emitter->random.GetF32Range(-1.0f, 1.0f);
            ptcl->pos.y = scaleY * emitter->random.GetF32Range(-1.0f, 1.0f);
            ptcl->pos.z = scaleZ * emitter->random.GetF32Range(-1.0f, 1.0f);
        }
        else
        {
            f32 v1 = emitter->random.GetF32();
            f32 v2 = 1.0f - v0;
            f32 v3 = v2 * v0;
            v1 *= 1.0f - v0*v0*v0;

            if (v1 < v2)
            {
                ptcl->pos.x = emitter->random.GetF32();
                ptcl->pos.y = emitter->random.GetF32() * v2 + v0;
                ptcl->pos.z = emitter->random.GetF32();
            }
            else if (v1 < v2 + v3)
            {
                ptcl->pos.x = emitter->random.GetF32() * v2 + v0;
                ptcl->pos.y = emitter->random.GetF32() * v0;
                ptcl->pos.z = emitter->random.GetF32();
            }
            else
            {
                ptcl->pos.x = emitter->random.GetF32() * v0;
                ptcl->pos.y = emitter->random.GetF32() * v0;
                ptcl->pos.z = emitter->random.GetF32() * v2 + v0;
            }

            if (emitter->random.GetF32() < 0.5f)
                ptcl->pos.x = -ptcl->pos.x;

            if (emitter->random.GetF32() < 0.5f)
                ptcl->pos.y = -ptcl->pos.y;

            if (emitter->random.GetF32() < 0.5f)
                ptcl->pos.z = -ptcl->pos.z;

            ptcl->pos.x *= scaleX;
            ptcl->pos.y *= scaleY;
            ptcl->pos.z *= scaleZ;
        }

        math::VEC3 normalizedVel;
        if (ptcl->pos.x != 0.0f || ptcl->pos.y != 0.0f || ptcl->pos.z != 0.0f)
        {
            normalizedVel = ptcl->pos;
            normalizedVel.Normalize();
        }

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = normalizedVel.y * velocityMag;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitLine(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[22];
    f32 center = data->lineCenter * scaleZ;

    EMIT_LOOP_START()

        ptcl->pos.x = 0.0f;
        ptcl->pos.y = 0.0f;
        ptcl->pos.z = emitter->random.GetF32() * scaleZ - (scaleZ + center) / 2.0f;

        ptcl->velocity.x = 0.0f;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitLineSameDivide(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[22];
    f32 center = data->lineCenter * scaleZ;

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

void EmitterCalc::CalcEmitRectangle(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->emitterVolumeScale.x * emitter->anim[22];
    f32 scaleZ = data->volumeScale.z * emitterSet->emitterVolumeScale.z * emitter->anim[23];

    EMIT_LOOP_START()

        u32 v0 = emitter->random.GetU32();
        u32 v1 = emitter->random.GetU32();

        math::VEC3 rndVec = {
            .x = emitter->random.GetF32Range(-1.0f, 1.0f),
            .y = 0.0f,
            .z = emitter->random.GetF32Range(-1.0f, 1.0f),
        };

        if (v0 < 0x7fffffff)
        {
            ptcl->pos.x = scaleX * rndVec.x;
            ptcl->pos.y = 0.0f;
            ptcl->pos.z = (v1 < 0x7fffffff) ? scaleZ : -scaleZ;
        }
        else // if (v0 < 0xFFFFFFFF)
        {
            ptcl->pos.x = (v1 < 0x7fffffff) ? scaleX : -scaleX;
            ptcl->pos.y = 0.0f;
            ptcl->pos.z = scaleZ * rndVec.z;
        }

        math::VEC3 normalizedVel;
        if (ptcl->pos.x != 0.0f || ptcl->pos.y != 0.0f || ptcl->pos.z != 0.0f)
        {
            normalizedVel = ptcl->pos;
            normalizedVel.Normalize();
        }

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = 0.0f;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

void EmitterCalc::CalcEmitPrimitive(EmitterInstance* emitter)
{
    if (emitter->volumePrimitive == NULL)
        return;

    EMIT_FUNCTION_START()
    (void)emitterSet; // Suppress unused warning

    math::VEC3* pos = static_cast<math::VEC3*>(emitter->volumePrimitive->vbPos.buffer);
    math::VEC3* normal = static_cast<math::VEC3*>(emitter->volumePrimitive->vbNormal.buffer);
    u32 numVertices = emitter->volumePrimitive->vbPos.bufferSize / sizeof(math::VEC3);

    if (emitter->data->primitiveEmitType == 0)
    {
        for (u32 i = 0; i < numVertices; i++)
        {
            PtclInstance* ptcl = mSys->AllocPtcl(emitter);
            if (ptcl == NULL)
                return;

            ptcl->pos = *pos++;
            ptcl->velocity = *normal++ * velocityMag;

        EMIT_LOOP_FUNCTION_END()
    }
    else if (emitter->data->primitiveEmitType == 1)
    {
        for (s32 i = 0; i < numEmit; i++)
        {
            u32 idx = (u32)(numVertices * emitter->random.GetF32());

            PtclInstance* ptcl = mSys->AllocPtcl(emitter);
            if (ptcl == NULL)
                return;

            ptcl->pos = pos[idx];
            ptcl->velocity = normal[idx] * velocityMag;

        EMIT_LOOP_FUNCTION_END()
    }
    else if (emitter->data->primitiveEmitType == 2)
    {
        for (s32 i = 0; i < numEmit; i++)
        {
            u32 idx = emitter->primitiveEmitCounter++ % numVertices;

            PtclInstance* ptcl = mSys->AllocPtcl(emitter);
            if (ptcl == NULL)
                return;

            ptcl->pos = pos[idx];
            ptcl->velocity = normal[idx] * velocityMag;

        EMIT_LOOP_FUNCTION_END()
    }

    emitter->emitterBehaviorFlg |= EmitterBehaviorFlag_IsEmitted;
}

} } // namespace nw::eft
