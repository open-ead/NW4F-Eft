#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

u32 EmitterCalc::CalcSimpleParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, CpuCore core)
{
    const SimpleEmitterData* data = emitter->data;

    math::VEC3 posBefore = ptcl->pos;
    math::MTX34* matrixSRT = ptcl->pMatrixSRT;

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

        ptcl->pos += ptcl->velocity * ptcl->randomUnk * emitter->emissionSpeed;

        if (emitter->particleBehaviorFlg & 1)
            ptcl->velocity *= data->airResist + (1.0f - data->airResist) * emissionSpeedInv;

        if (emitter->particleBehaviorFlg & 2)
        {
            math::VEC3 gravity = data->gravity * emitter->emissionSpeed;

            if (data->_283 != 0)
                ptcl->velocity += *math::VEC3::MultMTX(&gravity, &gravity, matrixRT);

            else
                ptcl->velocity += gravity;
        }

        if (emitter->particleBehaviorFlg & 0x80)
        {
            // EmitterCalc::ptclAnim_Alpha0_4k3v(EmitterInstance*, PtclInstance*, f32)
            {
                if (counter <= ptcl->alphaAnim->time2)
                    ptcl->alpha += ptcl->alphaAnim->startDiff * emitter->emissionSpeed;

                else if (counter > ptcl->alphaAnim->time3)
                    ptcl->alpha += ptcl->alphaAnim->endDiff * emitter->emissionSpeed;
            }
        }

        if (emitter->particleBehaviorFlg & 0x40)
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

        if (emitter->particleBehaviorFlg & 4)
            ptcl->rotation += ptcl->angularVelocity * emitter->emissionSpeed;

        if (emitter->particleBehaviorFlg & 8)
            math::VEC3::Scale(&ptcl->angularVelocity, &ptcl->angularVelocity, data->rotInertia + (1.0f - data->rotInertia) * emissionSpeedInv);

        if (emitter->particleBehaviorFlg & 0x100)
        {
            // Color0 Anim...
        }

        if (emitter->particleBehaviorFlg & 0x200)
        {
            // Color1 Anim...
        }

        if (emitter->particleBehaviorFlg & 0x1000)
        {
            // Tex0 Ptn Anim...
        }

        if (emitter->particleBehaviorFlg & 0x400)
        {
            // Tex0 UV Shift Anim...
        }

        if (emitter->particleBehaviorFlg & 0x4000 && emitter->particleBehaviorFlg & 0x2000)
        {
            // Tex1 Ptn Anim...
        }

        if (emitter->particleBehaviorFlg & 0x4000 && emitter->particleBehaviorFlg & 0x800)
        {
            // Tex1 UV Shift Anim...
        }
    }

    math::MTX34::MultVec(&ptcl->worldPos, matrixSRT, &ptcl->pos);

    if (emitter->particleBehaviorFlg & 0x10)
    {
        math::VEC3 posDiff = ptcl->pos - posBefore;
        if (fabsf(posDiff.x) > 0.0001 || fabsf(posDiff.y) > 0.0001 || fabsf(posDiff.z) > 0.0001)
            ptcl->posDiff += posDiff - ptcl->posDiff;

        math::MTX34::MultVecSR(&ptcl->worldPosDiff, matrixSRT, &ptcl->posDiff);
    }

    ptcl->counter += emitter->emissionSpeed;
    return 0;
}

u32 EmitterCalc::CalcComplexParticleBehavior(EmitterInstance* emitter, PtclInstance* ptcl, CpuCore core)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);

    math::VEC3 posBefore = ptcl->pos;
    math::MTX34* matrixSRT = ptcl->pMatrixSRT;

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

        ptcl->pos += ptcl->velocity * ptcl->randomUnk * emitter->emissionSpeed;

        if (emitter->particleBehaviorFlg & 1)
            ptcl->velocity *= data->airResist + (1.0f - data->airResist) * emissionSpeedInv;

        if (emitter->particleBehaviorFlg & 2)
        {
            math::VEC3 gravity = data->gravity * emitter->emissionSpeed;

            if (data->_283 != 0)
                ptcl->velocity += *math::VEC3::MultMTX(&gravity, &gravity, matrixRT);

            else
                ptcl->velocity += gravity;
        }

        if (emitter->particleBehaviorFlg & 0x80)
        {
            // EmitterCalc::ptclAnim_Alpha0_4k3v(EmitterInstance*, PtclInstance*, f32)
            {
                if (counter <= ptcl->alphaAnim->time2)
                    ptcl->alpha += ptcl->alphaAnim->startDiff * emitter->emissionSpeed;

                else if (counter > ptcl->alphaAnim->time3)
                    ptcl->alpha += ptcl->alphaAnim->endDiff * emitter->emissionSpeed;
            }
        }

        if (emitter->particleBehaviorFlg & 0x40)
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

        if (emitter->particleBehaviorFlg & 4)
            ptcl->rotation += ptcl->angularVelocity * emitter->emissionSpeed;

        if (emitter->particleBehaviorFlg & 8)
            math::VEC3::Scale(&ptcl->angularVelocity, &ptcl->angularVelocity, data->rotInertia + (1.0f - data->rotInertia) * emissionSpeedInv);

        if (emitter->particleBehaviorFlg & 0x100)
        {
            // Color0 Anim...
        }

        if (emitter->particleBehaviorFlg & 0x200)
        {
            // Color1 Anim...
        }

        if (emitter->particleBehaviorFlg & 0x1000)
        {
            // Tex0 Ptn Anim...
        }

        if (emitter->particleBehaviorFlg & 0x400)
        {
            // Tex0 UV Shift Anim...
        }

        if (emitter->particleBehaviorFlg & 0x4000 && emitter->particleBehaviorFlg & 0x2000)
        {
            // Tex1 Ptn Anim...
        }

        if (emitter->particleBehaviorFlg & 0x4000 && emitter->particleBehaviorFlg & 0x800)
        {
            // Tex1 UV Shift Anim...
        }
    }

    // Fluctuation and Field data...

    math::VEC3 posDiff = ptcl->pos - posBefore;
    if (fabsf(posDiff.x) > 0.0001 || fabsf(posDiff.y) > 0.0001 || fabsf(posDiff.z) > 0.0001)
        ptcl->posDiff += posDiff - ptcl->posDiff;

    math::MTX34::MultVec(&ptcl->worldPos, matrixSRT, &ptcl->pos);
    if (emitter->particleBehaviorFlg & 0x10)
        math::MTX34::MultVecSR(&ptcl->worldPosDiff, matrixSRT, &ptcl->posDiff);

    ptcl->counter += emitter->emissionSpeed;
    return 0;
}

void EmitterCalc::MakeParticleAttributeBuffer(PtclAttributeBuffer* ptclAttributeBuffer, PtclInstance* ptcl, u32 shaderAvailableAttribFlg, f32 cameraOffset)
{
    const EmitterSet* emitterSet = ptcl->emitter->emitterSet;

    ptclAttributeBuffer->wldPos.xyz() = ptcl->worldPos;
    ptclAttributeBuffer->wldPos.w = cameraOffset;

    ptclAttributeBuffer->scl.x = ptcl->scale.x * emitterSet->_220.x * ptcl->_B0;
    ptclAttributeBuffer->scl.y = ptcl->scale.y * emitterSet->_220.y * ptcl->_B0;
    ptclAttributeBuffer->scl.z = ptcl->texAnimParam[0].rotate;
    ptclAttributeBuffer->scl.w = ptcl->texAnimParam[1].rotate;

    ptclAttributeBuffer->color0.xyz() = ptcl->color0.rgb();
    ptclAttributeBuffer->color0.w = ptcl->alpha * ptcl->_AC;

    if (shaderAvailableAttribFlg & 0x40)
    {
        ptclAttributeBuffer->color1.xyz() = ptcl->color1.rgb();
        ptclAttributeBuffer->color1.w = ptcl->alpha * ptcl->_AC;
    }

    ptclAttributeBuffer->texAnim.x    = ptcl->texAnimParam[0].offset.x + ptcl->texAnimParam[0].scroll.x;
    ptclAttributeBuffer->texAnim.y    = ptcl->texAnimParam[0].offset.y - ptcl->texAnimParam[0].scroll.y;
    ptclAttributeBuffer->texAnim.zw() = ptcl->texAnimParam[0].scale;

    if (shaderAvailableAttribFlg & 0x10)
        ptclAttributeBuffer->wldPosDf.xyz() = ptcl->worldPosDiff;

    if (shaderAvailableAttribFlg & 0x80)
    {
        ptclAttributeBuffer->rot.xyz() = ptcl->rotation;
        ptclAttributeBuffer->rot.w = 0.0f;
    }

    if (shaderAvailableAttribFlg & 4)
    {
        ptclAttributeBuffer->subTexAnim.x    = ptcl->texAnimParam[1].offset.x + ptcl->texAnimParam[1].scroll.x;
        ptclAttributeBuffer->subTexAnim.y    = ptcl->texAnimParam[1].offset.y - ptcl->texAnimParam[1].scroll.y;
        ptclAttributeBuffer->subTexAnim.zw() = ptcl->texAnimParam[1].scale;
    }

    if (shaderAvailableAttribFlg & 0x100)
        ptclAttributeBuffer->emtMat = *ptcl->pMatrixRT;
}

} } // namespace nw::eft
