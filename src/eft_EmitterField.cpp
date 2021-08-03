#include <math/math_Triangular.h>
#include <eft_Animation.h>
#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_System.h>

namespace nw { namespace eft {

const void* EmitterCalc::_ptclField_Random(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed)
{
    const FieldRandomData* randomData = static_cast<const FieldRandomData*>(fieldData);

    if (ptcl->counterS32 % randomData->period == 0)
    {
        math::VEC3 randomVec3 = emitter->random.GetVec3();
        math::VEC3 randomVelScale;

        if (emitter->ptclAnimArray[28 - (27 + 1)] != NULL)
        {
            KeyFrameAnim* velScaleXAnim = emitter->ptclAnimArray[28 - (27 + 1)];
            KeyFrameAnim* velScaleYAnim = emitter->ptclAnimArray[29 - (27 + 1)];
            KeyFrameAnim* velScaleZAnim = emitter->ptclAnimArray[30 - (27 + 1)];

            f32 time = _calcParticleAnimTime(emitter, ptcl, 28);
            u32 index = CalcAnimKeyFrameIndex(velScaleXAnim, time);

            randomVelScale.x = CalcAnimKeyFrameSimple(velScaleXAnim, time, index);
            randomVelScale.y = CalcAnimKeyFrameSimple(velScaleYAnim, time, index);
            randomVelScale.y = CalcAnimKeyFrameSimple(velScaleZAnim, time, index);
        }
        else
        {
            randomVelScale = randomData->randomVelScale;
        }

        ptcl->velocity.x += randomVec3.x * randomVelScale.x;
        ptcl->velocity.y += randomVec3.y * randomVelScale.y;
        ptcl->velocity.z += randomVec3.z * randomVelScale.z;
    }

    return randomData + 1;
}

const void* EmitterCalc::_ptclField_Magnet(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed)
{
    const FieldMagnetData* magnetData = static_cast<const FieldMagnetData*>(fieldData);
    f32 strength;

    if (emitter->ptclAnimArray[31 - (27 + 1)] != NULL)
    {
        KeyFrameAnim* magnetAnim = emitter->ptclAnimArray[31 - (27 + 1)];

        f32 time = _calcParticleAnimTime(magnetAnim, ptcl, 31);
        u32 index = CalcAnimKeyFrameIndex(magnetAnim, time);

        strength = CalcAnimKeyFrameSimple(magnetAnim, time, index);
    }
    else
    {
        strength = magnetData->strength;
    }

    if (magnetData->followEmitter != 0)
    {
        math::VEC3 localPosCurr;

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

        if (magnetData->flags & 1) ptcl->velocity.x += (localPosCurr.x + magnetData->pos.x - ptcl->pos.x - ptcl->velocity.x) * strength;
        if (magnetData->flags & 2) ptcl->velocity.y += (localPosCurr.x + magnetData->pos.y - ptcl->pos.y - ptcl->velocity.y) * strength;
        if (magnetData->flags & 4) ptcl->velocity.z += (localPosCurr.x + magnetData->pos.z - ptcl->pos.z - ptcl->velocity.z) * strength;

        return magnetData + 1;
    }

    if (magnetData->flags & 1) ptcl->velocity.x += (magnetData->pos.x - ptcl->pos.x - ptcl->velocity.x) * strength;
    if (magnetData->flags & 2) ptcl->velocity.y += (magnetData->pos.y - ptcl->pos.y - ptcl->velocity.y) * strength;
    if (magnetData->flags & 4) ptcl->velocity.z += (magnetData->pos.z - ptcl->pos.z - ptcl->velocity.z) * strength;

    return magnetData + 1;
}

const void* EmitterCalc::_ptclField_Spin(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed)
{
    const FieldSpinData* spinData = static_cast<const FieldSpinData*>(fieldData);

    f32 sin_val, cos_val;
    u32 angle;
    f32 diffusionVel;

    if (emitter->ptclAnimArray[32 - (27 + 1)] != NULL)
    {
        KeyFrameAnim* spinAngleAnim = emitter->ptclAnimArray[32 - (27 + 1)];

        f32 time = _calcParticleAnimTime(spinAngleAnim, ptcl, 32);
        u32 index = CalcAnimKeyFrameIndex(spinAngleAnim, time);

        angle = CalcAnimKeyFrameSimpleS32(spinAngleAnim, time, index);
    }
    else
    {
        angle = spinData->angle;
    }

    if (emitter->ptclAnimArray[33 - (27 + 1)] != NULL)
    {
        KeyFrameAnim* spinDiffusionVelAnim = emitter->ptclAnimArray[33 - (27 + 1)];

        f32 time = _calcParticleAnimTime(spinDiffusionVelAnim, ptcl, 33);
        u32 index = CalcAnimKeyFrameIndex(spinDiffusionVelAnim, time);

        diffusionVel = CalcAnimKeyFrameSimple(spinDiffusionVelAnim, time, index);
    }
    else
    {
        diffusionVel = spinData->diffusionVel;
    }

    f32 degree = angle * (1.0f / 4294967296.0f) * 360.0f;
    if (degree > 180.0f)
        degree -= 360.0f;
    degree *= ptcl->randomF32 * emissionSpeed;
    if (degree < 0)
        degree += 360.0f;

    angle = (u32)(angle / 360.0f * 4294967296.0f);
    math::SinCosIdx(&sin_val, &cos_val, angle);

    switch (spinData->axis)
    {
    case 0:
        {
            f32 y, z;
            ptcl->pos.y = (y = ptcl->pos.y *  cos_val + ptcl->pos.z * sin_val);
            ptcl->pos.z = (z = ptcl->pos.y * -sin_val + ptcl->pos.z * cos_val);

            if (diffusionVel == 0.0f)
                break;

            f32 magSq = y*y + z*z;
            if (magSq <= 0.0f)
                break;

            f32 a = 1.0f / sqrtf(magSq) * diffusionVel * ptcl->randomF32 * emissionSpeed;
            ptcl->pos.y += y * a;
            ptcl->pos.z += z * a;
            break;
        }
    case 1:
        {
            f32 z, x;
            ptcl->pos.z = (z = ptcl->pos.z *  cos_val + ptcl->pos.x * sin_val);
            ptcl->pos.x = (x = ptcl->pos.z * -sin_val + ptcl->pos.x * cos_val);

            if (diffusionVel == 0.0f)
                break;

            f32 magSq = z*z + x*x;
            if (magSq <= 0.0f)
                break;

            f32 a = 1.0f / sqrtf(magSq) * diffusionVel * ptcl->randomF32 * emissionSpeed;
            ptcl->pos.z += z * a;
            ptcl->pos.x += x * a;
            break;
        }
    case 2:
        {
            f32 x, y;
            ptcl->pos.x = (x = ptcl->pos.x *  cos_val + ptcl->pos.y * sin_val);
            ptcl->pos.y = (y = ptcl->pos.x * -sin_val + ptcl->pos.y * cos_val);

            if (diffusionVel == 0.0f)
                break;

            f32 magSq = x*x + y*y;
            if (magSq <= 0.0f)
                break;

            f32 a = 1.0f / sqrtf(magSq) * diffusionVel * ptcl->randomF32 * emissionSpeed;
            ptcl->pos.x += x * a;
            ptcl->pos.y += y * a;
        }
    }

    return spinData + 1;
}

const void* EmitterCalc::_ptclField_Collision(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed)
{
    const FieldCollisionData* collisionData = static_cast<const FieldCollisionData*>(fieldData);

    if (collisionData->bounceCount != -1 && collisionData->bounceCount <= ptcl->fieldCollisionCounter)
        return collisionData + 1;

    f32 y = emitter->fieldCollisionY;

    if (collisionData->sharedPlane != 0)
    {
        if (emitter->emitterSet->system->isSharedPlaneEnable == 0)
            return collisionData + 1;

        System* system = emitter->emitterSet->system;

        f32 minX = system->sharedPlaneX.x;
        f32 maxX = system->sharedPlaneX.y;
        y        = system->sharedPlaneY;
        f32 minZ = system->sharedPlaneZ.x;
        f32 maxZ = system->sharedPlaneZ.y;

        if (collisionData->coordSystem != 0)
        {
            const math::MTX34* matrixSRT;
            if (emitter->ptclFollowType == PtclFollowType_SRT)
                matrixSRT = &emitter->matrixSRT;
            else
                matrixSRT = &ptcl->matrixSRT;

            math::VEC3 worldPos;
            math::MTX34::PSMultVec(&worldPos, matrixSRT, &ptcl->pos);

            switch (collisionData->collisionType)
            {
            case 0:
                if (worldPos.y < y && minX < worldPos.x && worldPos.x < maxX
                                   && minZ < worldPos.z && worldPos.z < maxZ)
                {
                    worldPos.y = y;
                    ptcl->counter = (f32)ptcl->lifespan;
                    ptcl->counterS32 = ptcl->lifespan;
                }
                break;
            case 1:
                if (worldPos.y < y && minX < worldPos.x && worldPos.x < maxX
                                   && minZ < worldPos.z && worldPos.z < maxZ)
                {
                    worldPos.y = y + 0.0001f;

                    math::VEC3 worldVelocity;
                    math::MTX34::MultVecSR(&worldVelocity, matrixSRT, &ptcl->velocity);

                    worldVelocity.y *= -collisionData->bounceRate;

                    math::MTX34 matrixSRTInv;
                    math::MTX34::Inverse(&matrixSRTInv, matrixSRT);

                    math::MTX34::PSMultVec(&ptcl->pos, &matrixSRTInv, &worldPos);
                    math::MTX34::MultVecSR(&ptcl->velocity, &matrixSRTInv, &worldVelocity);

                    ptcl->velocity *= collisionData->friction;
                    ptcl->fieldCollisionCounter++;
                }
            }
        }
        else
        {
            switch (collisionData->collisionType)
            {
            case 0:
                if (ptcl->pos.y < y && minX < ptcl->pos.x && ptcl->pos.x < maxX
                                    && minZ < ptcl->pos.z && ptcl->pos.z < maxZ)
                {
                    ptcl->pos.y = y;
                    ptcl->counter = (f32)ptcl->lifespan;
                    ptcl->counterS32 = ptcl->lifespan;
                }
                break;
            case 1:
                if (ptcl->pos.y < y && minX < ptcl->pos.x && ptcl->pos.x < maxX
                                    && minZ < ptcl->pos.z && ptcl->pos.z < maxZ)
                {
                    ptcl->pos.y = y;
                    ptcl->velocity.y *= -collisionData->bounceRate;

                    ptcl->velocity *= collisionData->friction;
                    ptcl->fieldCollisionCounter++;
                }
            }
        }
    }
    else if (collisionData->coordSystem != 0)
    {
        const math::MTX34* matrixSRT;
        if (emitter->ptclFollowType == PtclFollowType_SRT)
            matrixSRT = &emitter->matrixSRT;
        else
            matrixSRT = &ptcl->matrixSRT;

        math::VEC3 worldPos;
        math::MTX34::PSMultVec(&worldPos, matrixSRT, &ptcl->pos);

        switch (collisionData->collisionType)
        {
        case 0:
            if (worldPos.y < y)
            {
                worldPos.y = y;
                ptcl->counter = (f32)ptcl->lifespan;
                ptcl->counterS32 = ptcl->lifespan;
            }
            break;
        case 1:
            if (worldPos.y < y)
            {
                worldPos.y = y + 0.0001f;

                math::VEC3 worldVelocity;
                math::MTX34::MultVecSR(&worldVelocity, matrixSRT, &ptcl->velocity);

                worldVelocity.y *= -collisionData->bounceRate;

                math::MTX34 matrixSRTInv;
                math::MTX34::Inverse(&matrixSRTInv, matrixSRT);

                math::MTX34::PSMultVec(&ptcl->pos, &matrixSRTInv, &worldPos);
                math::MTX34::MultVecSR(&ptcl->velocity, &matrixSRTInv, &worldVelocity);

                ptcl->velocity *= collisionData->friction;
                ptcl->fieldCollisionCounter++;
            }
        }
    }
    else
    {
        switch (collisionData->collisionType)
        {
        case 0:
            if (ptcl->pos.y < y)
            {
                ptcl->pos.y = y;
                ptcl->counter = (f32)ptcl->lifespan;
                ptcl->counterS32 = ptcl->lifespan;
            }
            break;
        case 1:
            if (ptcl->pos.y < y)
            {
                ptcl->pos.y = y;
                ptcl->velocity.y *= -collisionData->bounceRate;

                ptcl->velocity *= collisionData->friction;
                ptcl->fieldCollisionCounter++;
            }
        }
    }

    return collisionData + 1;
}

const void* EmitterCalc::_ptclField_Convergence(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed)
{
    const FieldConvergenceData* convergenceData = static_cast<const FieldConvergenceData*>(fieldData);
    f32 strength;

    if (emitter->ptclAnimArray[34 - (27 + 1)] != NULL)
    {
        KeyFrameAnim* convergenceAnim = emitter->ptclAnimArray[34 - (27 + 1)];

        f32 time = _calcParticleAnimTime(convergenceAnim, ptcl, 34);
        u32 index = CalcAnimKeyFrameIndex(convergenceAnim, time);

        strength = CalcAnimKeyFrameSimple(convergenceAnim, time, index);
    }
    else
    {
        strength = convergenceData->strength;
    }

    if (convergenceData->followType != 0)
    {
        math::VEC3 localPosCurr;

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

        ptcl->pos += (localPosCurr + convergenceData->pos - ptcl->pos) * strength * ptcl->randomF32 * emissionSpeed;

        return convergenceData + 1;
    }

    ptcl->pos += (convergenceData->pos - ptcl->pos) * strength * ptcl->randomF32 * emissionSpeed;

    return convergenceData + 1;
}

const void* EmitterCalc::_ptclField_PosAdd(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed)
{
    const FieldPosAddData* posAddData = static_cast<const FieldPosAddData*>(fieldData);
    math::VEC3 posAdd;

    if (emitter->ptclAnimArray[35 - (27 + 1)] != NULL)
    {
        KeyFrameAnim* posAddXAnim = emitter->ptclAnimArray[35 - (27 + 1)];
        KeyFrameAnim* posAddYAnim = emitter->ptclAnimArray[36 - (27 + 1)];
        KeyFrameAnim* posAddZAnim = emitter->ptclAnimArray[37 - (27 + 1)];

        f32 time = _calcParticleAnimTime(posAddXAnim, ptcl, 35);
        u32 index = CalcAnimKeyFrameIndex(posAddXAnim, time);

        posAdd.x = CalcAnimKeyFrameSimple(posAddXAnim, time, index);
        posAdd.y = CalcAnimKeyFrameSimple(posAddYAnim, time, index);
        posAdd.z = CalcAnimKeyFrameSimple(posAddZAnim, time, index);
    }
    else
    {
        posAdd = posAddData->posAdd;
    }

    if (posAddData->coordSystem != 0)
    {
        posAdd = posAdd * ptcl->randomF32 * emissionSpeed;
        ptcl->pos += *math::VEC3::MultMTX(&posAdd, &posAdd, ptcl->pMatrixRT);

        return posAddData + 1;
    }

    ptcl->pos += posAdd * ptcl->randomF32 * emissionSpeed;

    return posAddData + 1;
}

} } // namespace nw::eft
