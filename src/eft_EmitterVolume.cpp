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

#include "eft_EmitterVolume.hpp"

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
        arcStartAngle = emitter->random.GetF32() * math::F_PI * 2.0f;

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

PtclInstance* EmitterCalc::CalcEmitSphere(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    f32 arcLength, arcStartAngle;

    if (data->_28A != 0)
    {
        arcLength = 0.0f;
        arcStartAngle = 0.0f;
    }
    else
    {
        arcLength = math::Idx2Rad(data->arcLength);
        arcStartAngle = (data->_287 != 0) ? emitter->random.GetF32() * math::F_PI * 2.0f
                                          : math::Idx2Rad(data->arcStartAngle);
    }

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = (data->_28A != 0) ? emitter->random.GetF32() * math::F_PI * 2.0f
                                                        : emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        f32 y = (data->_28A != 0) ? math::CosRad(emitter->random.GetF32() * data->_368)
                                  : emitter->random.GetF32Range(-1.0f, 1.0f);

        f32 a = 1.0f - y * y;
        if (a <= 0.0f)
            a = 0.0f;
        else
            a = sqrtf(a);

        math::VEC3 normalizedVel = (math::VEC3){ a * sin_val, y, a * cos_val };

        if (data->_28A != 0 && (data->_36C.x != 0.0f || data->_36C.y != 1.0f || data->_36C.z != 0.0f))
        {
            math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
            math::MTX34 mtx;
            math::MTX34::MakeVectorRotation(&mtx, &base, &data->_36C);

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

PtclInstance* EmitterCalc::CalcEmitSphereSameDivide(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    const math::VEC3* table = gSameDivideSphereTbl[data->_286];

    EMIT_LOOP_START()

        math::VEC3 normalizedVel = *table++;

        if (data->_28A != 0)
        {
            if (!(math::CosRad(data->_368) < normalizedVel.y))
                continue;

            if (data->_36C.x != 0.0f || data->_36C.y != 1.0f || data->_36C.z != 0.0f)
            {
                math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
                math::MTX34 mtx;
                math::MTX34::MakeVectorRotation(&mtx, &base, &data->_36C);

                math::MTX34::PSMultVec(&normalizedVel, &mtx, &normalizedVel);
            }
        }

        ptcl->pos.x = normalizedVel.x * scaleX;
        ptcl->pos.y = normalizedVel.y * scaleY;
        ptcl->pos.z = normalizedVel.z * scaleZ;

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = normalizedVel.y * velocityMag;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitSphereSameDivide64(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    const math::VEC3* table = gSameDivideSphere64Tbl[data->_286 + 2]; // What is the point of the first 2 tables?

    EMIT_LOOP_START()

        math::VEC3 normalizedVel = *table++;

        if (data->_28A != 0)
        {
            if (!(math::CosRad(data->_368) < normalizedVel.y))
                continue;

            if (data->_36C.x != 0.0f || data->_36C.y != 1.0f || data->_36C.z != 0.0f)
            {
                math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
                math::MTX34 mtx;
                math::MTX34::MakeVectorRotation(&mtx, &base, &data->_36C);

                math::MTX34::PSMultVec(&normalizedVel, &mtx, &normalizedVel);
            }
        }

        ptcl->pos.x = normalizedVel.x * scaleX;
        ptcl->pos.y = normalizedVel.y * scaleY;
        ptcl->pos.z = normalizedVel.z * scaleZ;

        ptcl->velocity.x = normalizedVel.x * velocityMag;
        ptcl->velocity.y = normalizedVel.y * velocityMag;
        ptcl->velocity.z = normalizedVel.z * velocityMag;

    EMIT_LOOP_FUNCTION_END()
}

PtclInstance* EmitterCalc::CalcEmitFillSphere(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    f32 arcLength, arcStartAngle;

    if (data->_28A != 0)
    {
        arcLength = 0.0f;
        arcStartAngle = 0.0f;
    }
    else
    {
        arcLength = math::Idx2Rad(data->arcLength);
        arcStartAngle = (data->_287 != 0) ? emitter->random.GetF32() * math::F_PI * 2.0f
                                          : math::Idx2Rad(data->arcStartAngle);
    }

    EMIT_LOOP_START()

        f32 sin_val, cos_val, angle = (data->_28A != 0) ? emitter->random.GetF32() * math::F_PI * 2.0f
                                                        : emitter->random.GetF32() * arcLength + arcStartAngle;
        math::SinCosRad(&sin_val, &cos_val, angle);

        f32 y = (data->_28A != 0) ? math::CosRad(emitter->random.GetF32() * data->_368)
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
        extent = extent * data->_364 + 1.0f - data->_364;

        math::VEC3 normalizedVel = (math::VEC3){ a * sin_val, y, a * cos_val };

        if (data->_28A != 0 && (data->_36C.x != 0.0f || data->_36C.y != 1.0f || data->_36C.z != 0.0f))
        {
            math::VEC3 base = (math::VEC3){ 0.0f, 1.0f, 0.0f };
            math::MTX34 mtx;
            math::MTX34::MakeVectorRotation(&mtx, &base, &data->_36C);

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

PtclInstance* EmitterCalc::CalcEmitCylinder(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    f32 arcLength = math::Idx2Rad(data->arcLength);
    f32 arcStartAngle = math::Idx2Rad(data->arcStartAngle);
    if (data->_287 != 0)
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

PtclInstance* EmitterCalc::CalcEmitFillCylinder(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    f32 arcLength = math::Idx2Rad(data->arcLength);
    f32 arcStartAngle = math::Idx2Rad(data->arcStartAngle);
    if (data->_287 != 0)
        arcStartAngle = emitter->random.GetF32() * math::F_PI * 2.0f;

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

PtclInstance* EmitterCalc::CalcEmitBox(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

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

PtclInstance* EmitterCalc::CalcEmitFillBox(EmitterInstance* emitter)
{
    EMIT_FUNCTION_START()

    f32 scaleX = data->volumeScale.x * emitterSet->_228.x * emitter->anim[22];
    f32 scaleY = data->volumeScale.y * emitterSet->_228.y * emitter->anim[23];
    f32 scaleZ = data->volumeScale.z * emitterSet->_228.z * emitter->anim[24];

    EMIT_LOOP_START()

        f32 v0 = 1.0f - data->_364;
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
