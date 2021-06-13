#include <eft_EmitterComplex.h>
#include <eft_EmitterSet.h>
#include <eft_Renderer.h>
#include <eft_System.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

u32 EmitterComplexCalc::CalcParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer)
{
    System* system = emitter->emitterSet->system;
    VertexTransformMode vertexTransformMode = emitter->data->vertexTransformMode;

    if (!noMakePtclAttributeBuffer
        && vertexTransformMode != VertexTransformMode_Stripe
        && vertexTransformMode != VertexTransformMode_Complex_Stripe)
    {
        Renderer** const renderers = system->renderers;

        emitter->ptclAttributeBuffer = static_cast<PtclAttributeBuffer*>(renderers[core]->AllocFromDoubleBuffer(sizeof(PtclAttributeBuffer) * emitter->numParticles));
        if (emitter->ptclAttributeBuffer == NULL)
            return 0;

        emitter->emitterDynamicUniformBlock = static_cast<EmitterDynamicUniformBlock*>(renderers[core]->AllocFromDoubleBuffer(sizeof(EmitterDynamicUniformBlock)));
        if (emitter->emitterDynamicUniformBlock == NULL)
        {
            emitter->ptclAttributeBuffer = NULL;
            return 0;
        }

        math::VEC3 emitterSetColor = emitter->emitterSet->color.rgb();
        emitterSetColor.x *= emitter->data->colorScaleFactor;
        emitterSetColor.y *= emitter->data->colorScaleFactor;
        emitterSetColor.z *= emitter->data->colorScaleFactor;

        emitter->emitterDynamicUniformBlock->emitterColor0.x = emitterSetColor.x * emitter->anim[11];
        emitter->emitterDynamicUniformBlock->emitterColor0.y = emitterSetColor.y * emitter->anim[12];
        emitter->emitterDynamicUniformBlock->emitterColor0.z = emitterSetColor.z * emitter->anim[13];
        emitter->emitterDynamicUniformBlock->emitterColor0.w = emitter->emitterSet->color.a * emitter->anim[14] * emitter->fadeAlpha;

        emitter->emitterDynamicUniformBlock->emitterColor1.x = emitterSetColor.x * emitter->anim[19];
        emitter->emitterDynamicUniformBlock->emitterColor1.y = emitterSetColor.y * emitter->anim[20];
        emitter->emitterDynamicUniformBlock->emitterColor1.z = emitterSetColor.z * emitter->anim[21];
        emitter->emitterDynamicUniformBlock->emitterColor1.w = emitter->emitterSet->color.a * emitter->anim[14] * emitter->fadeAlpha;

        GX2EndianSwap(emitter->emitterDynamicUniformBlock, sizeof(EmitterDynamicUniformBlock));
    }
    else
    {
        emitter->ptclAttributeBuffer = NULL;
        emitter->emitterDynamicUniformBlock = NULL;
    }

    emitter->numDrawParticle = 0;

    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);

    CustomActionParticleCalcCallback callback1 = mSys->GetCurrentCustomActionParticleCalcCallback(emitter);
    CustomActionParticleMakeAttributeCallback callback2 = mSys->GetCurrentCustomActionParticleMakeAttributeCallback(emitter);

    PtclInstance* ptcl = emitter->particleHead;
    bool reversed = false;

    if (emitter->data->flags & 0x400)
    {
        ptcl = emitter->particleTail;
        reversed = true;
    }

    for (; ptcl != NULL; ptcl = reversed ? ptcl->prev : ptcl->next)
    {
        if (ptcl->data == NULL)
            continue;

        if (!noCalcBehavior)
        {
            if (ptcl->lifespan <= (s32)ptcl->counter || ptcl->lifespan == 1 && ptcl->counter != 0.0f)
            {
                PtclStripe* stripe = ptcl->stripe;
                if (stripe != NULL)
                {
                    const StripeData* stripeData = reinterpret_cast<const StripeData*>((u32)data + data->stripeDataOffs);

                    if (stripe->queueFront == stripe->queueRear)
                        RemoveParticle(emitter, ptcl, core);

                    else
                    {
                        if (++stripe->queueFront >= stripeData->queueCount)
                            stripe->queueFront = 0;

                        stripe->queueCount--;
                        stripe->emitterMatrixSRT = emitter->matrixSRT;

                        ptcl->texAnimParam[0].scroll.x += data->texAnimParam[0].texIncScroll.x;
                        ptcl->texAnimParam[0].scroll.y += data->texAnimParam[0].texIncScroll.y;
                        ptcl->texAnimParam[0].scale.x  += data->texAnimParam[0].texIncScale.x;
                        ptcl->texAnimParam[0].scale.y  += data->texAnimParam[0].texIncScale.y;
                        ptcl->texAnimParam[0].rotate   += data->texAnimParam[0].texIncRotate;

                        ptcl->texAnimParam[1].scroll.x += data->texAnimParam[1].texIncScroll.x;
                        ptcl->texAnimParam[1].scroll.y += data->texAnimParam[1].texIncScroll.y;
                        ptcl->texAnimParam[1].scale.x  += data->texAnimParam[1].texIncScale.x;
                        ptcl->texAnimParam[1].scale.y  += data->texAnimParam[1].texIncScale.y;
                        ptcl->texAnimParam[1].rotate   += data->texAnimParam[1].texIncRotate;

                        emitter->numDrawParticle++;
                    }

                    stripe->counter++;
                }
                else
                {
                    RemoveParticle(emitter, ptcl, core);
                }

                continue;
            }

            CalcComplexParticleBehavior(emitter, ptcl, core);
            CalcComplexParticle(emitter, ptcl, core);
        }

        if (callback1 != NULL)
        {
            ParticleCalcArg arg = {
                .emitter = emitter,
                .ptcl = ptcl,
                .core = core,
                .noCalcBehavior = noCalcBehavior,
            };
            callback1(arg);
        }

        if (!noMakePtclAttributeBuffer
            && vertexTransformMode != VertexTransformMode_Stripe
            && vertexTransformMode != VertexTransformMode_Complex_Stripe)
        {
            MakeParticleAttributeBuffer(&emitter->ptclAttributeBuffer[emitter->numDrawParticle], ptcl, emitter->shaderAvailableAttribFlg, emitter->data->cameraOffset);
            ptcl->ptclAttributeBuffer = &emitter->ptclAttributeBuffer[emitter->numDrawParticle++];

            if (callback2 != NULL)
            {
                ParticleMakeAttrArg arg = {
                    .emitter = emitter,
                    .ptcl = ptcl,
                    .core = core,
                    .noCalcBehavior = noCalcBehavior,
                };
                callback2(arg);
            }
        }
    }

    if (data->vertexTransformMode == VertexTransformMode_Stripe)
        system->renderers[core]->MakeStripeAttributeBlock(emitter);

    emitter->isCalculated = true;
    return emitter->numDrawParticle;
}

} } // namespace nw::eft
