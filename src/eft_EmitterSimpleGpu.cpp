#include <eft_EmitterSet.h>
#include <eft_EmitterSimpleGpu.h>
#include <eft_System.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

void EmitterSimpleGpuCalc::CalcEmitter(EmitterInstance* emitter)
{
    if (emitter->ptclAttributeBufferGpu == NULL)
        return;

    const EmitterSet* emitterSet = emitter->emitterSet;
    const SimpleEmitterData* data = emitter->data;
    bool emit;

    if (emitter->numUsedAnim != 0)
        ApplyAnim(emitter);

    math::MTX34::Concat(&emitter->matrixRT,  &emitterSet->matrixRT,  &emitter->animMatrixRT);
    math::MTX34::Concat(&emitter->matrixSRT, &emitterSet->matrixSRT, &emitter->animMatrixSRT);

    if (emitter->counter == 0.0f)
    {
        emitter->prevPos.x = emitter->matrixRT.m[0][3];
        emitter->prevPos.y = emitter->matrixRT.m[1][3];
        emitter->prevPos.z = emitter->matrixRT.m[2][3];
    }

    EmitterMatrixSetArg arg = { .emitter = emitter };
    CustomActionEmitterMatrixSetCallback callback = mSys->GetCurrentCustomActionEmitterMatrixSetCallback(emitter);
    if (callback != NULL)
        callback(arg);

    if (emitterSet->doFade != 0)
    {
        emit = !data->noEmitAtFade;

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

    s32 time = (s32)emitter->counter - emitterSet->startFrame;
    f32 interval = emitter->emissionInterval * emitter->controller->emissionInterval;

    if (time < data->endFrame && emit)
    {
        if (time >= data->startFrame)
        {
            if (!(emitter->emitterBehaviorFlg & EmitterBehaviorFlag_IsEmitted))
            {
                mEmitFunctions[data->emitFunction](emitter);
                emitter->preCalcCounter = emitter->counter;

                if (interval == 0.0f)
                    goto calc_done;
            }

            if (data->emitSameDistance == 0)
            {
                f32 numEmit;

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
                            numEmit = 0.0f;
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
                            numEmit = 0.0f;
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

                        if (data->ptclMaxLifespan == 1 && emitter->emissionSpeed == 1.0f)
                            numEmit = 1.0f;
                    }
                }

                for (s32 i = 0; i < (s32)numEmit; i++)
                {
                    if (emitterSet->numEmissionPoints > 0)
                    {
                        for (s32 j = 0; j < emitterSet->numEmissionPoints; j++)
                        {
                            PtclInstance* particleHeadBefore = emitter->particleHead;
                            mEmitFunctions[data->emitFunction](emitter);
                            for (PtclInstance* ptcl = emitter->particleHead; ptcl != particleHeadBefore; ptcl = ptcl->next)
                                ptcl->pos += emitterSet->emissionPoints[j];
                        }
                    }
                    else
                    {
                        mEmitFunctions[data->emitFunction](emitter);
                    }
                }

                emitter->emissionInterval = data->emitInterval + emitter->random.GetS32(data->emitIntervalRandom);

                if ((s32)numEmit != 0 && emitter->data->updateEmitterInfoByEmit != 0)
                {
                    emitter->UpdateEmitterInfoByEmit();
                    if (callback != NULL)
                        callback(arg);
                }
            }
            else
            {
                EmitSameDistance(data, emitter);
            }
        }
    }
    else
    {
        if (data->oneTime != 0)
        {
            if (data->endFrame + data->ptclMaxLifespan + 1u < (u32)time)
                return mSys->KillEmitter(emitter);
        }
        else
        {
            emitter->gpuCounter += emitter->emissionSpeed;
            if (data->ptclMaxLifespan < emitter->gpuCounter)
                return mSys->KillEmitter(emitter);
        }
    }

calc_done:
    emitter->emitterBehaviorFlg |= EmitterBehaviorFlag_PrevPosSet;
    emitter->prevPos.x = emitter->matrixRT.m[0][3];
    emitter->prevPos.y = emitter->matrixRT.m[1][3];
    emitter->prevPos.z = emitter->matrixRT.m[2][3];

    emitter->counter2 = emitter->counter;
    emitter->counter += emitter->emissionSpeed;

    u32 firstIdx = emitter->currentPtclAttributeBufferGpuIdx;
    PtclInstance* ptcl = emitter->particleHead;
    PtclInstance* next;

    for (; ptcl != NULL; ptcl = next)
    {
        PtclAttributeBufferGpu* ptclAttributeBufferGpu = &emitter->ptclAttributeBufferGpu[emitter->currentPtclAttributeBufferGpuIdx];

        ptclAttributeBufferGpu->wldPos.xyz() = ptcl->pos;
        ptclAttributeBufferGpu->wldPos.w         = ptcl->emitStartFrame;

        ptclAttributeBufferGpu->scl.xy() = ptcl->scale;

        ptclAttributeBufferGpu->vec.xyz() = ptcl->velocity;
        ptclAttributeBufferGpu->vec.w = ptcl->randomF32;

        ptclAttributeBufferGpu->random.xyz() = ptcl->randomVec4.xyz();
        ptclAttributeBufferGpu->random.w = ptcl->lifespan;

        ptclAttributeBufferGpu->rot.xyz() = ptcl->rotation;
        ptclAttributeBufferGpu->rot.w = 0.0f;

        ptclAttributeBufferGpu->emtMat = *ptcl->pMatrixSRT;

        if (++emitter->currentPtclAttributeBufferGpuIdx == emitter->numPtclAttributeBufferGpuMax)
        {
            DCFlushRange(&emitter->ptclAttributeBufferGpu[firstIdx], sizeof(PtclAttributeBufferGpu) * (emitter->numPtclAttributeBufferGpuMax - firstIdx));
            emitter->currentPtclAttributeBufferGpuIdx = 0;
            firstIdx = 0;
        }

        if (emitter->numDrawParticle < emitter->numPtclAttributeBufferGpuMax)
            emitter->numDrawParticle++;

        next = ptcl->next;
        RemoveParticle(ptcl, CpuCore_1);
    }

    DCFlushRange(&emitter->ptclAttributeBufferGpu[firstIdx], sizeof(PtclAttributeBufferGpu) * (emitter->currentPtclAttributeBufferGpuIdx - firstIdx));
    emitter->numParticles = 1;
}

u32 EmitterSimpleGpuCalc::CalcParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer)
{
    if (noMakePtclAttributeBuffer)
        return emitter->numDrawParticle;

    emitter->emitterDynamicUniformBlock = MakeEmitterUniformBlock(emitter, core, NULL, noCalcBehavior);
    if (emitter->emitterDynamicUniformBlock == NULL)
        return 0;

    emitter->emitterBehaviorFlg |= EmitterBehaviorFlag_IsCalculated;
    return emitter->numDrawParticle;
}

} } // namespace nw::eft
