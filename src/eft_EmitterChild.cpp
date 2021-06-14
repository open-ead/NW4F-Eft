#include <eft_EmitterComplex.h>
#include <eft_Particle.h>
#include <eft_System.h>

namespace nw { namespace eft {

void EmitterComplexCalc::EmitChildParticle(EmitterInstance* emitter, PtclInstance* ptcl)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);
    const ChildData* childData = reinterpret_cast<const ChildData*>(data + 1);

    for (s32 i = 0; i < childData->numChildParticles; i++)
    {
        PtclInstance* childPtcl = mSys->AllocPtcl(PtclType_Child);
        if (childPtcl == NULL)
            continue;

        childPtcl->data = data;
        childPtcl->stripe = NULL;
        childPtcl->lifespan = childData->ptclMaxLifespan;
        childPtcl->emitter = emitter;

        if (data->childFlags & 0x20)
            childPtcl->velocity = ptcl->posDiff * childData->velocityScaleFactor;

        else
            childPtcl->velocity = math::VEC3::Zero();

        f32 alpha;

        if (data->childFlags & 4)
            alpha = ptcl->alpha * ptcl->_AC;

        else
            alpha = childData->_184;

        f32 scaleRandom = 1.0f - childData->_19C * emitter->random.GetF32();

        if (data->childFlags & 8)
        {
            childPtcl->scale.x = ptcl->scale.x * childData->_190 * ptcl->_B0 * scaleRandom * emitter->anim[17];
            childPtcl->scale.y = ptcl->scale.y * childData->_190 * ptcl->_B0 * scaleRandom * emitter->anim[18];
        }
        else
        {
            childPtcl->scale.x = scaleRandom * childData->_194.x;
            childPtcl->scale.y = scaleRandom * childData->_194.y;
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
            childPtcl->rotation.x = emitter->random.GetF32() * childData->_1B0.x + childData->_1A4.x;
            childPtcl->rotation.y = emitter->random.GetF32() * childData->_1B0.y + childData->_1A4.y;
            childPtcl->rotation.z = emitter->random.GetF32() * childData->_1B0.z + childData->_1A4.z;
        }

        if (data->childFlags & 2)
        {
            childPtcl->color0 = ptcl->color0;
        }
        else
        {
            childPtcl->color0.rgb() = childData->_160;
            childPtcl->color0.a     = 0.0f;
        }

        if (data->childFlags & 0x8000)
        {
            childPtcl->color1 = ptcl->color1;
        }
        else
        {
            childPtcl->color1.rgb() = childData->_16C;
            childPtcl->color1.a     = 0.0f;
        }

        childPtcl->angularVelocity.x = emitter->random.GetF32() * childData->_1C8.x + childData->_1BC.x;
        childPtcl->angularVelocity.y = emitter->random.GetF32() * childData->_1C8.y + childData->_1BC.y;
        childPtcl->angularVelocity.z = emitter->random.GetF32() * childData->_1C8.z + childData->_1BC.z;

        childPtcl->texAnimParam[0].scroll = (math::VEC2){ 0.0f, 0.0f };
        childPtcl->texAnimParam[0].rotate = 0.0f;
        childPtcl->texAnimParam[0].scale = (math::VEC2){ 0.0f, 0.0f };

        if (childData->_1F0 == 0)
        {
            childPtcl->alphaAnim->startDiff = 0.0f;
            childPtcl->alpha = alpha;
        }
        else
        {
            childPtcl->alphaAnim->startDiff = (alpha - childData->_18C) / (f32)childData->_1F0;
            childPtcl->alpha = childData->_18C;
        }
        childPtcl->alphaAnim->endDiff = (childData->_188 - alpha) / (f32)(childPtcl->lifespan - childData->_1EC);

        f32 scaleAnimDurationInv = 1.0f / (childPtcl->lifespan - childData->_1F4);
        childPtcl->scaleAnim->startDiff.x = (childData->_1F8.x - 1.0f) * scaleAnimDurationInv * childPtcl->scale.x;
        childPtcl->scaleAnim->startDiff.y = (childData->_1F8.y - 1.0f) * scaleAnimDurationInv * childPtcl->scale.y;

        childPtcl->_AC = 1.0f;
        childPtcl->_B0 = 1.0f;

        if (childData->texPtnAnimIdxRand > 1)
        {
            s32 texPtnAnimIdx = emitter->random.GetU32(childData->texPtnAnimIdxRand);
            s32 offsetX = texPtnAnimIdx % childData->texPtnAnimIdxDiv;
            s32 offsetY = texPtnAnimIdx / childData->texPtnAnimIdxDiv;

            childPtcl->texAnimParam[0].offset.x = childData->uvScaleInit.x * (f32)offsetX;
            childPtcl->texAnimParam[0].offset.y = childData->uvScaleInit.y * (f32)offsetY;
        }
        else
        {
            childPtcl->texAnimParam[0].offset = (math::VEC2){ 0.0f, 0.0f };
        }

        math::VEC3 randomNormVec3 = emitter->random.GetNormalizedVec3();
        math::VEC3 randomVec3 = emitter->random.GetVec3();

        childPtcl->velocity.x += randomNormVec3.x * childData->_14 + randomVec3.x * childData->_18.x;
        childPtcl->velocity.y += randomNormVec3.y * childData->_14 + randomVec3.y * childData->_18.y;
        childPtcl->velocity.z += randomNormVec3.z * childData->_14 + randomVec3.z * childData->_18.z;

        childPtcl->pos = randomNormVec3 * childData->_24 + ptcl->pos;

        childPtcl->counter = 0.0f;
        childPtcl->randomU32 = emitter->random.GetU32();

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
        childPtcl->randomF32 = 1.0f - childData->_34 * emitter->random.GetF32Range(-1.0f, 1.0f);
        childPtcl->_140 = 0;

        CustomActionParticleEmitCallback callback = mSys->GetCurrentCustomActionParticleEmitCallback(emitter);
        if (callback != NULL)
        {
            ParticleEmitArg arg = { .ptcl = childPtcl };
            if (!callback(arg))
                return RemoveParticle(emitter, childPtcl, CpuCore_1);
        }

        AddChildPtclToList(emitter, childPtcl);
    }
}

} } // namespace nw::eft
