#include <eft_EmitterSet.h>
#include <eft_EmitterSimple.h>
#include <eft_Renderer.h>
#include <eft_System.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

void EmitterSimpleCalc::CalcEmitter(EmitterInstance* emitter)
{
    const EmitterSet* emitterSet = emitter->emitterSet;
    const SimpleEmitterData* data = emitter->data;
    bool emit;

    if (emitterSet->doFade != 0)
    {
        emit = !data->_285;

        emitter->fadeAlpha -= data->fadeAlphaStep;
        if (emitter->fadeAlpha <= 0.0f)
            return mSys->KillEmitter(emitter);
    }
    else
    {
        emit = true;

        emitter->fadeAlpha += data->fadeAlphaStep;
        if (emitter->fadeAlpha > 1.0f)
            emitter->fadeAlpha = 1.0f;
    }

    ApplyAnim(emitter);
    UpdateEmitterInfoByEmit(emitter);

    s32 time = (s32)emitter->counter - emitterSet->startFrame;
    if (time < data->endFrame && emit)
    {
        if (time >= data->startFrame)
        {
            if (data->_289 == 0)
            {
                f32 interval = emitter->emissionInterval * emitter->controller->_4;

                f32 numEmit, numEmit2 = 0.0f;
                if (emitter->isEmitted == 0)
                    numEmit2 = 1.0f;

                if ((s32)interval != 0)
                {
                    if (emitter->emissionSpeed >= 1.0f)
                    {
                        if ((s32)emitter->emitCounter >= interval)
                        {
                            numEmit = 1.0f;
                            emitter->emitCounter = 0.0f;
                        }
                        else
                        {
                            numEmit = numEmit2;
                            emitter->emitCounter += emitter->emissionSpeed;
                        }
                    }
                    else
                    {
                        if (emitter->emitCounter >= interval && (s32)emitter->counter != (s32)emitter->counter2)
                        {
                            numEmit = 1.0f;
                            emitter->emitCounter = 0.0f;
                            emitter->emitLostTime = 0.0f;
                            emitter->preCalcCounter = emitter->counter;
                        }
                        else
                        {
                            numEmit = numEmit2;
                            emitter->emitCounter += emitter->emissionSpeed;
                        }
                    }
                }
                else
                {
                    if (emitter->emissionSpeed >= 1.0f)
                        numEmit = 1.0f;

                    else
                    {
                        numEmit = emitter->counter - emitter->preCalcCounter + emitter->emitLostTime;

                        if (emitter->emitLostTime >= interval)
                            emitter->emitLostTime -= interval;

                        emitter->emitLostTime += emitter->counter - emitter->preCalcCounter - (s32)numEmit;
                        emitter->emitCounter = 0.0f;
                        emitter->preCalcCounter = emitter->counter;

                        if (data->ptclMaxLifespan == 1)
                            numEmit = 1.0f;
                    }
                }

                for (s32 i = 0; i < (s32)numEmit; i++)
                {
                    if (emitterSet->_278 > 0)
                    {
                        for (s32 j = 0; j < emitterSet->_278; j++)
                        {
                            PtclInstance* particleHeadBefore = emitter->particleHead;
                            mEmitFunctions[data->_34C](emitter);
                            for (PtclInstance* ptcl = emitter->particleHead; ptcl != particleHeadBefore; ptcl = ptcl->next)
                                ptcl->pos += emitterSet->_27C[j];
                        }
                    }
                    else
                    {
                        mEmitFunctions[data->_34C](emitter);
                    }
                }

                emitter->emissionInterval = data->emitInterval + emitter->random.GetS32(data->emitIntervalRandom);
            }
            else
            {
                EmitSameDistance(data, emitter);
            }
        }
    }
    else
    {
        if (emitter->particleHead == NULL && emitter->childParticleHead == NULL)
            return mSys->KillEmitter(emitter);
    }

    emitter->counter2 = emitter->counter;
    emitter->counter += emitter->emissionSpeed;

    if (emitter->particleHead != NULL)
    {
        emitter->prevPos.x = emitter->matrixRT.m[0][3];
        emitter->prevPos.y = emitter->matrixRT.m[1][3];
        emitter->prevPos.z = emitter->matrixRT.m[2][3];
        emitter->prevPosSet = true;
    }
}

u32 EmitterSimpleCalc::CalcParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer)
{
    if (!noMakePtclAttributeBuffer)
    {
        Renderer** const renderers = emitter->emitterSet->system->renderers;

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
                RemoveParticle(emitter, ptcl, core);
                continue;
            }

            CalcSimpleParticleBehavior(emitter, ptcl, core);
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

        if (!noMakePtclAttributeBuffer)
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

    emitter->isCalculated = true;
    return emitter->numDrawParticle;
}

} } // namespace nw::eft
