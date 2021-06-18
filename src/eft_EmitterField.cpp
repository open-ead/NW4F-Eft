#include <math/math_Triangular.h>
#include <eft_Emitter.h>

namespace nw { namespace eft {

const void* EmitterCalc::_ptclField_Random(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData)
{
    const FieldRandomData* randomData = static_cast<const FieldRandomData*>(fieldData);

    if ((s32)ptcl->counter % randomData->period == 0)
    {
        math::VEC3 randomVec3 = emitter->random.GetVec3();
        ptcl->velocity.x += randomVec3.x * randomData->randomVelScale.x;
        ptcl->velocity.y += randomVec3.y * randomData->randomVelScale.y;
        ptcl->velocity.z += randomVec3.z * randomData->randomVelScale.z;
    }

    return randomData + 1;
}

const void* EmitterCalc::_ptclField_Magnet(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData)
{
    const FieldMagnetData* magnetData = static_cast<const FieldMagnetData*>(fieldData);

    if (magnetData->flags & 1) ptcl->velocity.x += (magnetData->pos.x - ptcl->pos.x - ptcl->velocity.x) * magnetData->strength;
    if (magnetData->flags & 2) ptcl->velocity.y += (magnetData->pos.y - ptcl->pos.y - ptcl->velocity.y) * magnetData->strength;
    if (magnetData->flags & 4) ptcl->velocity.z += (magnetData->pos.z - ptcl->pos.z - ptcl->velocity.z) * magnetData->strength;

    return magnetData + 1;
}

const void* EmitterCalc::_ptclField_Spin(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData)
{
    const FieldSpinData* spinData = static_cast<const FieldSpinData*>(fieldData);

    f32 sin_val, cos_val;
    u32 angle = (u32)(spinData->angle * emitter->emissionSpeed * ptcl->randomF32);
    math::SinCosIdx(&sin_val, &cos_val, angle);

    switch (spinData->axis)
    {
    case 0:
        {
            f32 y, z;
            ptcl->pos.y = (y = ptcl->pos.y *  cos_val + ptcl->pos.z * sin_val);
            ptcl->pos.z = (z = ptcl->pos.y * -sin_val + ptcl->pos.z * cos_val);

            if (spinData->diffusionVel == 0.0f)
                break;

            f32 magSq = y*y + z*z;
            if (magSq <= 0.0f)
                break;

            f32 a = 1.0f / sqrtf(magSq) * spinData->diffusionVel * ptcl->randomF32 * emitter->emissionSpeed; // * emitter->emissionSpeed <-- mistake?
            ptcl->pos.y += y * a * emitter->emissionSpeed;
            ptcl->pos.z += z * a * emitter->emissionSpeed;
            break;
        }
    case 1:
        {
            f32 z, x;
            ptcl->pos.z = (z = ptcl->pos.z *  cos_val + ptcl->pos.x * sin_val);
            ptcl->pos.x = (x = ptcl->pos.z * -sin_val + ptcl->pos.x * cos_val);

            if (spinData->diffusionVel == 0.0f)
                break;

            f32 magSq = z*z + x*x;
            if (magSq <= 0.0f)
                break;

            f32 a = 1.0f / sqrtf(magSq) * spinData->diffusionVel * ptcl->randomF32;
            ptcl->pos.z += z * a * emitter->emissionSpeed;
            ptcl->pos.x += x * a * emitter->emissionSpeed;
            break;
        }
    case 2:
        {
            f32 x, y;
            ptcl->pos.x = (x = ptcl->pos.x *  cos_val + ptcl->pos.y * sin_val);
            ptcl->pos.y = (y = ptcl->pos.x * -sin_val + ptcl->pos.y * cos_val);

            if (spinData->diffusionVel == 0.0f)
                break;

            f32 magSq = x*x + y*y;
            if (magSq <= 0.0f)
                break;

            f32 a = 1.0f / sqrtf(magSq) * spinData->diffusionVel * ptcl->randomF32;
            ptcl->pos.x += x * a * emitter->emissionSpeed;
            ptcl->pos.y += y * a * emitter->emissionSpeed;
        }
    }

    return spinData + 1;
}

const void* EmitterCalc::_ptclField_Collision(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData)
{
    const FieldCollisionData* collisionData = static_cast<const FieldCollisionData*>(fieldData);

    f32 y = collisionData->y;

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
            if (worldPos.y < y)
            {
                worldPos.y = y;
                ptcl->counter = (f32)ptcl->lifespan - emitter->emissionSpeed;
            }
            break;
        case 1:
            if (worldPos.y < y)
            {
                worldPos.y = y + 0.0001f;

                math::VEC3 worldVelocity;
                math::MTX34::MultVecSR(&worldVelocity, matrixSRT, &ptcl->velocity);

                worldVelocity.y *= -collisionData->friction;

                math::MTX34 matrixSRTInv;
                math::MTX34::Inverse(&matrixSRTInv, matrixSRT);

                math::MTX34::PSMultVec(&ptcl->pos, &matrixSRTInv, &worldPos);
                math::MTX34::MultVecSR(&ptcl->velocity, &matrixSRTInv, &worldVelocity);
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
                ptcl->counter = (f32)ptcl->lifespan - emitter->emissionSpeed;
            }
            break;
        case 1:
            if (ptcl->pos.y < y)
            {
                ptcl->pos.y = y;
                ptcl->velocity.y *= -collisionData->friction;
            }
        }
    }

    return collisionData + 1;
}

const void* EmitterCalc::_ptclField_Convergence(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData)
{
    const FieldConvergenceData* convergenceData = static_cast<const FieldConvergenceData*>(fieldData);

    ptcl->pos += (convergenceData->pos - ptcl->pos) * convergenceData->strength * ptcl->randomF32 * emitter->emissionSpeed;

    return convergenceData + 1;
}

const void* EmitterCalc::_ptclField_PosAdd(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData)
{
    const FieldPosAddData* posAddData = static_cast<const FieldPosAddData*>(fieldData);

    ptcl->pos += posAddData->posAdd * ptcl->randomF32 * emitter->emissionSpeed;

    return posAddData + 1;
}

} } // namespace nw::eft
