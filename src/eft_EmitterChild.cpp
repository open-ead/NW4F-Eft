#include <eft_EmitterComplex.h>
#include <eft_Particle.h>
#include <eft_System.h>

namespace nw { namespace eft {

static inline f32 _initialize3v4kAnimInherit(AlphaAnim* anim, const anim3v4Key* key, s32 lifespan, f32 alphaMid)
{
    anim->time2 = (s32)(key->time2 * lifespan);
    anim->time3 = (s32)(key->time3 * lifespan);

    if (anim->time2 == 0)
        anim->startDiff = 0.0f;

    else
        anim->startDiff = (alphaMid - key->startValue) / (f32)anim->time2;

    if (key->time3 == 1.0f)
        anim->endDiff = 0.0f;

    else
    {
        f32 alphaEnd = key->startValue + key->startDiff + key->endDiff;
        anim->endDiff = (alphaEnd - alphaMid) / (f32)(lifespan - anim->time3);
    }

    if (anim->time2 == 0)
        return alphaMid;

    return key->startValue - anim->startDiff;
}

void EmitterComplexCalc::EmitChildParticle(EmitterInstance* emitter, PtclInstance* ptcl)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);
    const ChildData* childData = reinterpret_cast<const ChildData*>(data + 1);

    for (s32 i = 0; i < childData->numChildParticles; i++)
    {
        PtclInstance* childPtcl = mSys->AllocPtcl();
        if (childPtcl == NULL)
            continue;

        childPtcl->data = data;
        childPtcl->cdata = data;
        childPtcl->complexParam->stripe = NULL;
        childPtcl->lifespan = childData->ptclMaxLifespan;
        childPtcl->emitter = emitter;
        childPtcl->counter = 0.0f;
        childPtcl->counterS32 = 0;
        childPtcl->type = PtclType_Child;
        childPtcl->randomU32 = emitter->random.GetU32();
        childPtcl->_unused = 0;
        childPtcl->fluctuationAlpha = 1.0f;
        childPtcl->fluctuationScale = (math::VEC2){ 1.0f, 1.0f };
        childPtcl->randomVec4.x = emitter->random.GetF32();
        childPtcl->randomVec4.y = emitter->random.GetF32();
        childPtcl->randomVec4.z = emitter->random.GetF32();
        childPtcl->randomVec4.w = emitter->random.GetF32();

        if (data->childFlags & 0x20)
            childPtcl->velocity = ptcl->posDiff * childData->velInheritRatio;

        else
            childPtcl->velocity = math::VEC3::Zero();

        math::VEC3 randomNormVec3 = emitter->random.GetNormalizedVec3();
        math::VEC3 randomVec3 = emitter->random.GetVec3();

        childPtcl->velocity.x += randomNormVec3.x * childData->allDirVel + randomVec3.x * childData->diffusionVel.x;
        childPtcl->velocity.y += randomNormVec3.y * childData->allDirVel + randomVec3.y * childData->diffusionVel.y;
        childPtcl->velocity.z += randomNormVec3.z * childData->allDirVel + randomVec3.z * childData->diffusionVel.z;

        childPtcl->posDiff = childPtcl->velocity;
        childPtcl->pos = randomNormVec3 * childData->ptclPosRandom + ptcl->pos - childPtcl->velocity;

        if (data->childFlags & 2)
        {
            childPtcl->color0 = ptcl->color0;
        }
        else
        {
            childPtcl->color0.rgb() = childData->ptclColor0;
            childPtcl->color0.a     = 0.0f;
        }

        if (data->childFlags & 0x8000)
        {
            childPtcl->color1 = ptcl->color1;
        }
        else
        {
            childPtcl->color1.rgb() = childData->ptclColor1;
            childPtcl->color1.a     = 1.0f;
        }

        if (data->childFlags & 0x10)
        {
            switch (childData->rotationMode)
            {
            case VertexRotationMode_Rotate_X:
                childPtcl->rotation.x = ptcl->rotation.x;
                childPtcl->rotation.y = 0.0f;
                childPtcl->rotation.z = 0.0f;
                break;
            case VertexRotationMode_Rotate_Y:
                childPtcl->rotation.x = 0.0f;
                childPtcl->rotation.y = ptcl->rotation.y;
                childPtcl->rotation.z = 0.0f;
                break;
            case VertexRotationMode_Rotate_Z:
                childPtcl->rotation.x = 0.0f;
                childPtcl->rotation.y = 0.0f;
                childPtcl->rotation.z = ptcl->rotation.z;
                break;
            //case VertexRotationMode_Rotate_XYZ:
            default:
                childPtcl->rotation = ptcl->rotation;
            }
        }
        else
        {
            childPtcl->rotation.x = childData->ptclRotate.x + emitter->random.GetF32() * childData->ptclRotateRandom.x;
            childPtcl->rotation.y = childData->ptclRotate.y + emitter->random.GetF32() * childData->ptclRotateRandom.y;
            childPtcl->rotation.z = childData->ptclRotate.z + emitter->random.GetF32() * childData->ptclRotateRandom.z;
        }

        childPtcl->angularVelocity.x = childData->angularVelocity.x + emitter->random.GetF32() * childData->angularVelocityRandom.x;
        childPtcl->angularVelocity.y = childData->angularVelocity.y + emitter->random.GetF32() * childData->angularVelocityRandom.y;
        childPtcl->angularVelocity.z = childData->angularVelocity.z + emitter->random.GetF32() * childData->angularVelocityRandom.z;

        if (data->childFlags & 0x10)
        {
            if (childData->rotateDirRandomX != 0 && emitter->random.GetF32() > 0.5f)
                childPtcl->angularVelocity.x = -childPtcl->angularVelocity.x;

            if (childData->rotateDirRandomY != 0 && emitter->random.GetF32() > 0.5f)
                childPtcl->angularVelocity.y = -childPtcl->angularVelocity.y;

            if (childData->rotateDirRandomZ != 0 && emitter->random.GetF32() > 0.5f)
                childPtcl->angularVelocity.z = -childPtcl->angularVelocity.z;
        }
        else
        {
            if (childData->rotateDirRandomX != 0 && emitter->random.GetF32() > 0.5f)
            {
                childPtcl->velocity.x = -childPtcl->velocity.x;
                childPtcl->angularVelocity.x = -childPtcl->angularVelocity.x;
            }

            if (childData->rotateDirRandomY != 0 && emitter->random.GetF32() > 0.5f)
            {
                childPtcl->velocity.y = -childPtcl->velocity.y;
                childPtcl->angularVelocity.y = -childPtcl->angularVelocity.y;
            }

            if (childData->rotateDirRandomZ != 0 && emitter->random.GetF32() > 0.5f)
            {
                childPtcl->velocity.z = -childPtcl->velocity.z;
                childPtcl->angularVelocity.z = -childPtcl->angularVelocity.z;
            }
        }

        s32 lifespan = childPtcl->lifespan - 1;

        if (data->childFlags & 0x400000)
        {
            if (childData->ptclAlphaSrc[0] == AlphaSourceType_First)
                childPtcl->alpha0 = ptcl->alpha0 * ptcl->fluctuationAlpha;

            else if (childData->ptclAlphaSrc[0] == AlphaSourceType_3v4k)
                childPtcl->alpha0 = _initialize3v4kAnimInherit(childPtcl->alphaAnim[0], &childData->alphaAnim[0], lifespan, ptcl->alpha0 * ptcl->fluctuationAlpha);
        }
        else
        {
            if (childData->ptclAlphaSrc[0] == AlphaSourceType_First)
                childPtcl->alpha0 = childData->alphaAnim[0].startValue + childData->alphaAnim[0].startDiff;

            else if (childData->ptclAlphaSrc[0] == AlphaSourceType_3v4k)
                childPtcl->alpha0 = _initialize3v4kAnim(childPtcl->alphaAnim[0], &childData->alphaAnim[0], lifespan);
        }

        if (data->childFlags & 0x800000)
        {
            if (childData->ptclAlphaSrc[1] == AlphaSourceType_First)
                childPtcl->alpha1 = ptcl->alpha1 * ptcl->fluctuationAlpha;

            else if (childData->ptclAlphaSrc[1] == AlphaSourceType_3v4k)
                childPtcl->alpha1 = _initialize3v4kAnimInherit(childPtcl->alphaAnim[1], &childData->alphaAnim[1], lifespan, ptcl->alpha1 * ptcl->fluctuationAlpha);
        }
        else
        {
            if (childData->ptclAlphaSrc[1] == AlphaSourceType_First)
                childPtcl->alpha1 = childData->alphaAnim[1].startValue + childData->alphaAnim[1].startDiff;

            else if (childData->ptclAlphaSrc[1] == AlphaSourceType_3v4k)
                childPtcl->alpha1 = _initialize3v4kAnim(childPtcl->alphaAnim[1], &childData->alphaAnim[1], lifespan);
        }

        f32 scaleRandom = 1.0f - childData->ptclScaleRandom * emitter->random.GetF32();

        if (data->childFlags & 8)
        {
            childPtcl->scale.x = ptcl->scale.x * childData->scaleInheritRatio * ptcl->fluctuationScale.x * scaleRandom * emitter->anim[17];
            childPtcl->scale.y = ptcl->scale.y * childData->scaleInheritRatio * ptcl->fluctuationScale.y * scaleRandom * emitter->anim[18];
        }
        else
        {
            childPtcl->scale.x = scaleRandom * childData->ptclEmitScale.x;
            childPtcl->scale.y = scaleRandom * childData->ptclEmitScale.y;
        }

        f32 scaleAnimDurationInv = 1.0f / (childPtcl->lifespan - childData->scaleAnimTime1);
        childPtcl->scaleAnim->startDiff.x = (childData->ptclScaleEnd.x - 1.0f) * scaleAnimDurationInv * childPtcl->scale.x;
        childPtcl->scaleAnim->startDiff.y = (childData->ptclScaleEnd.y - 1.0f) * scaleAnimDurationInv * childPtcl->scale.y;

        childPtcl->_13C = 0.0f;

        if (!(data->childFlags & 0x40))
        {
            if (emitter->ptclFollowType == PtclFollowType_SRT)
            {
                childPtcl->matrixRT = emitter->matrixRT;
                childPtcl->matrixSRT = emitter->matrixSRT;
                childPtcl->pMatrixRT = &emitter->matrixRT;
                childPtcl->pMatrixSRT = &emitter->matrixSRT;
            }
            else
            {
                childPtcl->matrixRT = ptcl->matrixRT;
                childPtcl->matrixSRT = ptcl->matrixSRT;
                childPtcl->pMatrixRT = &ptcl->matrixRT;
                childPtcl->pMatrixSRT = &ptcl->matrixSRT;
            }
        }
        else
        {
            childPtcl->pMatrixRT = &emitter->matrixRT;
            childPtcl->pMatrixSRT = &emitter->matrixSRT;
        }

        childPtcl->randomU32 = emitter->random.GetU32(); // Again... ?
        childPtcl->randomF32 = 1.0f - childData->momentumRandom * emitter->random.GetF32Range(-1.0f, 1.0f);

        CustomActionParticleEmitCallback callback = mSys->GetCurrentCustomActionParticleEmitCallback(emitter);
        if (callback != NULL)
        {
            ParticleEmitArg arg = { .ptcl = childPtcl };
            if (!callback(arg))
                return RemoveParticle(childPtcl, CpuCore_1);
        }

        AddChildPtclToList(emitter, childPtcl);
    }
}

} } // namespace nw::eft
