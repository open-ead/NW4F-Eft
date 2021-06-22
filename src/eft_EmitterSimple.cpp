#include <eft_Animation.h>
#include <eft_EmitterSet.h>
#include <eft_EmitterSimple.h>
#include <eft_Renderer.h>
#include <eft_System.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

void EmitterCalc::ApplyAnim(EmitterInstance* emitter)
{
    const SimpleEmitterData* data = emitter->data;
    KeyFrameAnimArray* animArray = emitter->animArray;

    math::MTX34::Copy(&emitter->animMatrixRT,  &math::MTX34::Identity());
    math::MTX34::Copy(&emitter->animMatrixSRT, &math::MTX34::Identity());

    math::MTX34::Concat(&emitter->animMatrixRT,  &emitter->animMatrixRT,  &data->animMatrixRT);
    math::MTX34::Concat(&emitter->animMatrixSRT, &emitter->animMatrixSRT, &data->animMatrixSRT);

    if (animArray == NULL)
    {
        math::VEC3 scale, rotate, translate;
        math::VEC3::Add(&scale, &data->emitterScale, &emitter->scaleRandom);
        math::VEC3::Add(&rotate, &data->emitterRotate, &emitter->rotateRandom);
        math::VEC3::Add(&translate, &data->emitterTranslate, &emitter->translateRandom);

        math::MTX34::MakeSRT(&emitter->animMatrixSRT, &scale, &rotate, &translate);
        math::MTX34::MakeRT(&emitter->animMatrixRT, &rotate, &translate);
    }
    else
    {
        KeyFrameAnim* anim = reinterpret_cast<KeyFrameAnim*>(animArray + 1);
        for (u32 i = 0; i < animArray->numAnim; i++)
        {
            emitter->anim[anim->animValIdx] = CalcAnimKeyFrame(anim, emitter->counter);
            anim = reinterpret_cast<KeyFrameAnim*>((u32)anim + anim->nextOffs);
        }

        math::VEC3 scale     = (math::VEC3){ emitter->anim[ 2], emitter->anim[ 3], emitter->anim[ 4] };
        math::VEC3 rotate    = (math::VEC3){ emitter->anim[ 5], emitter->anim[ 6], emitter->anim[ 7] };
        math::VEC3 translate = (math::VEC3){ emitter->anim[ 8], emitter->anim[ 9], emitter->anim[10] };

        math::MTX34::MakeSRT(&emitter->animMatrixSRT, &scale, &rotate, &translate);
        math::MTX34::MakeRT(&emitter->animMatrixRT, &rotate, &translate);
    }
}

void EmitterCalc::UpdateEmitterInfoByEmit(EmitterInstance* emitter)
{
    const EmitterSet* emitterSet = emitter->emitterSet;

    math::MTX34::Concat(&emitter->matrixRT,  &emitterSet->matrixRT,  &emitter->animMatrixRT);
    math::MTX34::Concat(&emitter->matrixSRT, &emitterSet->matrixSRT, &emitter->animMatrixSRT);
}

void EmitterSimpleCalc::EmitSameDistance(const SimpleEmitterData* data, EmitterInstance* emitter)
{
    if (!emitter->prevPosSet)
    {
        mEmitFunctions[data->_34C](emitter);
        return;
    }

    math::VEC3 prevPos = emitter->prevPos;
    math::VEC3 currPos = (math::VEC3){ emitter->matrixRT.m[0][3], emitter->matrixRT.m[1][3], emitter->matrixRT.m[2][3] };

    math::VEC3 moved;
    math::VEC3::Subtract(&moved, &prevPos, &currPos);
    f32 movedDist = moved.Magnitude();

    f32 lostDist = emitter->emitLostDistance;

    if (movedDist < data->_3B8 || movedDist == 0.0f)
        movedDist = data->_3B4;
    else if (movedDist < data->_3B4)
        movedDist = movedDist * data->_3B4 / movedDist;
    else if (data->_3B0 < movedDist)
        movedDist = movedDist * data->_3B0 / movedDist;

    f32 remainDist = lostDist + movedDist;
    s32 numEmit = (s32)(remainDist / data->_3AC); // No division-by-zero check

    for (s32 i = 0; i < numEmit; i++)
    {
        remainDist -= data->_3AC;

        f32 prevCurrRatio = 0.0f;
        if (movedDist != 0.0f)
            prevCurrRatio = remainDist / movedDist;

        math::VEC3 pos_0;
        math::VEC3 pos_1;
        math::VEC3 pos;

        math::VEC3::Scale(&pos_0, &currPos, 1.0f - prevCurrRatio);
        math::VEC3::Scale(&pos_1, &prevPos, prevCurrRatio);
        math::VEC3::Add(&pos, &pos_0, &pos_1);

        emitter->matrixRT.m[0][3] = pos.x;
        emitter->matrixRT.m[1][3] = pos.y;
        emitter->matrixRT.m[2][3] = pos.z;
        emitter->matrixSRT.m[0][3] = pos.x;
        emitter->matrixSRT.m[1][3] = pos.y;
        emitter->matrixSRT.m[2][3] = pos.z;

        mEmitFunctions[data->_34C](emitter);

        // No idea why this is done inside the loop and not after it
        emitter->matrixRT.m[0][3] = currPos.x;
        emitter->matrixRT.m[1][3] = currPos.y;
        emitter->matrixRT.m[2][3] = currPos.z;
        emitter->matrixSRT.m[0][3] = currPos.x;
        emitter->matrixSRT.m[1][3] = currPos.y;
        emitter->matrixSRT.m[2][3] = currPos.z;
    }

    emitter->emitLostDistance = remainDist;
}

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
