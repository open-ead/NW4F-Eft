#include <eft_EmitterSet.h>
#include <eft_EmitterSimple.h>
#include <eft_System.h>

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

    s32 time = (s32)emitter->counter - emitterSet->startFrame;
    if (time < data->endFrame && emit)
    {
        if (time >= data->startFrame)
        {
            if (data->_289 == 0)
            {
                f32 interval = emitter->emissionInterval * emitter->controller->_4;
                f32 numEmit2 = (emitter->isEmitted == 0) ? 1.0f : 0.0f;
                s32 numEmit;

                if ((s32)interval != 0)
                {
                    if (emitter->emissionSpeed >= 1.0f)
                    {
                        if ((s32)emitter->emitCounter >= interval)
                        {
                            numEmit = (s32)(1.0f); // ???
                            emitter->emitCounter = 0.0f;
                        }
                        else
                        {
                            numEmit = (s32)numEmit2;
                            emitter->emitCounter += emitter->emissionSpeed;
                        }
                    }
                    else
                    {
                        if (emitter->emitCounter >= interval && (s32)emitter->counter != (s32)emitter->counter2)
                        {
                            numEmit = (s32)(1.0f); // ???
                            emitter->emitCounter = 0.0f;
                            emitter->emitLostTime = 0.0f;
                            emitter->preCalcCounter = emitter->counter2;
                        }
                        else
                        {
                            numEmit = (s32)numEmit2;
                            emitter->emitCounter += emitter->emissionSpeed;
                        }
                    }
                }
                else
                {
                    if (emitter->emissionSpeed >= 1.0f)
                        numEmit = (s32)(1.0f); // ???

                    else
                    {
                        numEmit = (s32)(emitter->counter - emitter->preCalcCounter + emitter->emitLostTime);

                        if (emitter->emitLostTime >= interval)
                            emitter->emitLostTime -= interval;

                        emitter->emitLostTime += emitter->counter - emitter->preCalcCounter - numEmit;
                        emitter->emitCounter = 0.0f;
                        emitter->preCalcCounter = emitter->counter;

                        if (data->ptclMaxLifespan == 1)
                            numEmit = (s32)(1.0f); // ???
                    }
                }

                for (s32 i = 0; i < numEmit; i++)
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

                emitter->emissionInterval = data->emitInterval + (s32)emitter->random.GetU32(data->emitIntervalRandom);
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

} } // namespace nw::eft
