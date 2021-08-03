#include <eft_EmitterComplex.h>
#include <eft_EmitterSet.h>
#include <eft_Renderer.h>
#include <eft_Shader.h>
#include <eft_System.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

void EmitterComplexCalc::CalcStripe(EmitterInstance* emitter, PtclInstance* ptcl, const StripeData* stripeData, const ComplexEmitterData* data, CpuCore core, bool noMakePtclAttributeBuffer)
{
    s32 counter = ptcl->counterS32 - 1;
    System* system = emitter->emitterSet->system;

    PtclStripe* stripe = ptcl->complexParam->stripe;
    if (stripe == NULL)
        return;

    stripe->texAnimParam[0].scroll.x += data->texAnimParam[0].texIncScroll.x * emitter->emissionSpeed;
    stripe->texAnimParam[0].scroll.y += data->texAnimParam[0].texIncScroll.y * emitter->emissionSpeed;
    stripe->texAnimParam[0].scale.x  += data->texAnimParam[0].texIncScale.x  * emitter->emissionSpeed;
    stripe->texAnimParam[0].scale.y  += data->texAnimParam[0].texIncScale.y  * emitter->emissionSpeed;
    stripe->texAnimParam[0].rotate   += data->texAnimParam[0].texIncRotate   * emitter->emissionSpeed;

    stripe->texAnimParam[1].scroll.x += data->texAnimParam[1].texIncScroll.x * emitter->emissionSpeed;
    stripe->texAnimParam[1].scroll.y += data->texAnimParam[1].texIncScroll.y * emitter->emissionSpeed;
    stripe->texAnimParam[1].scale.x  += data->texAnimParam[1].texIncScale.x  * emitter->emissionSpeed;
    stripe->texAnimParam[1].scale.y  += data->texAnimParam[1].texIncScale.y  * emitter->emissionSpeed;
    stripe->texAnimParam[1].rotate   += data->texAnimParam[1].texIncRotate   * emitter->emissionSpeed;

    PtclStripeSliceHistory* currentSlice = &stripe->queue[stripe->queueRear];

    if (!(stripe->flags & 1) || stripe->queueRear < stripeData->numSliceHistory - 1)
    {
        if ((s32)(floorf(ptcl->counter + emitter->emissionSpeed) - floorf(ptcl->counter)) == 0)
            return;

        if (data->stripeFlags & 1)
        {
            currentSlice->pos = ptcl->pos;
            math::MTX34::Copy(&currentSlice->emitterMatrixSRT, &math::MTX34::Identity());
        }
        else
        {
            f32 sliceInterpolation = stripeData->sliceInterpolation;
            if (counter > 2 && stripeData->numSliceHistory > 3 && sliceInterpolation < 1.0f && stripe->queueRear != stripe->queueFront)
            {
                s32 prevIdx = stripe->queueRear - 1;
                if (prevIdx < 0)
                    prevIdx = stripeData->numSliceHistory - 1;

                s32 prev2Idx = prevIdx - 1;
                if (prev2Idx < 0)
                    prev2Idx = stripeData->numSliceHistory - 1;

                math::VEC3 diff0;
                math::VEC3::Subtract(&diff0, &ptcl->worldPos, &stripe->pos0);
                math::VEC3::Scale(&diff0, &diff0, sliceInterpolation);
                math::VEC3::Add(&stripe->pos0, &stripe->pos0, &diff0);

                math::VEC3 diff1;
                math::VEC3::Subtract(&diff1, &stripe->pos0, &stripe->pos1);
                math::VEC3::Scale(&diff1, &diff1, sliceInterpolation);
                math::VEC3::Add(&stripe->pos1, &stripe->pos1, &diff1);

                stripe->queue[prev2Idx].pos = stripe->pos1;

                math::VEC3 diff2;
                math::VEC3::Subtract(&diff2, &ptcl->worldPos, &stripe->pos1);
                math::VEC3::Scale(&diff2, &diff2, 0.7f);
                math::VEC3::Add(&stripe->queue[prevIdx].pos, &stripe->pos1, &diff2);

                currentSlice->pos = ptcl->worldPos;
            }
            else
            {
                stripe->pos0 = (stripe->pos1 = (currentSlice->pos = ptcl->worldPos));
            }

            currentSlice->emitterMatrixSRT = emitter->matrixSRT;
        }

        currentSlice->scale = ptcl->scale.x * emitter->emitterSet->ptclEffectiveScale.x;

        if (stripe->queueRear != stripe->queueFront)
        {
            s32 prevIdx = stripe->queueRear - 1;
            if (prevIdx < 0)
                prevIdx = stripeData->numSliceHistory - 1;

            PtclStripeSliceHistory* prevSlice = &stripe->queue[prevIdx];

            if (counter < 2)
            {
                math::VEC3::Subtract(&stripe->currentSliceDir, &currentSlice->pos, &prevSlice->pos);
                if (stripe->currentSliceDir.Magnitude() > 0.0f)
                    stripe->currentSliceDir.Normalize();
            }
            else
            {
                math::VEC3 posDiff;
                math::VEC3::Subtract(&posDiff, &currentSlice->pos, &prevSlice->pos);
                if (posDiff.Magnitude() > 0.0f)
                    posDiff.Normalize();

                math::VEC3 diff;
                math::VEC3::Subtract(&diff, &posDiff, &stripe->currentSliceDir);
                math::VEC3::Scale(&diff, &diff, stripeData->dirInterpolation);
                math::VEC3::Add(&stripe->currentSliceDir, &stripe->currentSliceDir, &diff);
                if (stripe->currentSliceDir.Magnitude() > 0.0f)
                    stripe->currentSliceDir.Normalize();
            }

            currentSlice->dir = stripe->currentSliceDir;

            if (stripeData->type == 2)
            {
                currentSlice->outer.x = currentSlice->emitterMatrixSRT.m[0][1];
                currentSlice->outer.y = currentSlice->emitterMatrixSRT.m[1][1];
                currentSlice->outer.z = currentSlice->emitterMatrixSRT.m[2][1];
            }
            else
            {
                math::VEC3 outer = (math::VEC3){ currentSlice->emitterMatrixSRT.m[0][1],
                                                 currentSlice->emitterMatrixSRT.m[1][1],
                                                 currentSlice->emitterMatrixSRT.m[2][1] };
                math::VEC3::CrossProduct(&outer, &outer, &stripe->currentSliceDir);
                if (outer.Magnitude() > 0.0f)
                    outer.Normalize();

                currentSlice->outer = outer;
            }
        }

        if (++stripe->queueRear >= stripeData->numSliceHistory)
            stripe->queueRear = 0;

        if (stripe->queueRear == stripe->queueFront
            && ++stripe->queueFront >= stripeData->numSliceHistory)
            stripe->queueFront = 0;

        if (++stripe->queueCount >= stripeData->numSliceHistory)
            stripe->queueCount = stripeData->numSliceHistory;
    }

    stripe->counter++;

    if (!noMakePtclAttributeBuffer)
        system->renderers[core]->MakeStripeAttributeBlock(emitter, ptcl);
}

void EmitterComplexCalc::EmitChildParticle(EmitterInstance* emitter, PtclInstance* ptcl, CpuCore core, const ChildData* childData)
{
    s32 counter = ptcl->counterS32;
    if (counter < ((ptcl->lifespan - 1) * childData->startFramePercent / 100))
        return;

    if (ptcl->complexParam->childEmitCounter >= childData->emissionInterval || childData->emissionInterval == 0 && childData->ptclMaxLifespan == 1)
    {
        if (ptcl->complexParam->childPreCalcCounter > 0.0f)
        {
            f32 time = emitter->counter - ptcl->complexParam->childPreCalcCounter + ptcl->complexParam->childEmitLostTime;
            if (childData->emissionInterval != 0)
                time /= childData->emissionInterval;

            if (ptcl->complexParam->childEmitLostTime >= childData->emissionInterval)
                ptcl->complexParam->childEmitLostTime -= childData->emissionInterval;

            ptcl->complexParam->childEmitLostTime += emitter->counter - ptcl->complexParam->childPreCalcCounter - (s32)time;
        }

        mSys->AddPtclAdditionList(ptcl, core);

        ptcl->complexParam->childEmitCounter = 0.0f;
        ptcl->complexParam->childPreCalcCounter = emitter->counter;
    }
    else
    {
        ptcl->complexParam->childEmitCounter += emitter->emissionSpeed;
    }
}

u32 EmitterComplexCalc::CalcParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer)
{
    emitter->numDrawParticle = 0;
    if (emitter->numParticles == 0)
        return 0;

    System* system = emitter->emitterSet->system;
    VertexTransformMode vertexTransformMode = emitter->data->vertexTransformMode;
    PtclAttributeBuffer* ptclAttributeBuffer = NULL;

    if (!noMakePtclAttributeBuffer
        && vertexTransformMode != VertexTransformMode_Stripe
        && vertexTransformMode != VertexTransformMode_Complex_Stripe)
    {
        Renderer** const renderers = system->renderers;

        emitter->ptclAttributeBuffer = static_cast<PtclAttributeBuffer*>(renderers[core]->AllocFromDoubleBuffer(sizeof(PtclAttributeBuffer) * emitter->numParticles));
        if (emitter->ptclAttributeBuffer == NULL)
        {
            emitter->emitterDynamicUniformBlock = NULL;
            return 0;
        }

        emitter->emitterDynamicUniformBlock = MakeEmitterUniformBlock(emitter, core, NULL, false);
        if (emitter->emitterDynamicUniformBlock == NULL)
        {
            emitter->ptclAttributeBuffer = NULL;
            return 0;
        }

        ptclAttributeBuffer = emitter->ptclAttributeBuffer;
    }
    else
    {
        emitter->ptclAttributeBuffer = NULL;
        emitter->emitterDynamicUniformBlock = NULL;
    }

    const ComplexEmitterData* data = emitter->GetComplexEmitterData();

    CustomActionParticleCalcCallback callback1 = mSys->GetCurrentCustomActionParticleCalcCallback(emitter);
    CustomActionParticleMakeAttributeCallback callback2 = mSys->GetCurrentCustomActionParticleMakeAttributeCallback(emitter);

    PtclInstance* ptcl = emitter->particleHead;
    const ChildData* childData = emitter->GetChildData();
    const StripeData* stripeData = NULL;
    u32 shaderAvailableAttribFlg = emitter->shader[ShaderType_Normal]->shaderAvailableAttribFlg;

    bool isStripe = false;
    if (vertexTransformMode == VertexTransformMode_Stripe
        || vertexTransformMode == VertexTransformMode_Complex_Stripe)
    {
        isStripe = true;
        stripeData = emitter->GetStripeData();
    }

    bool simpleStripe = vertexTransformMode == VertexTransformMode_Stripe;
    bool complexStripe = vertexTransformMode == VertexTransformMode_Complex_Stripe;

    f32 numBehaviorIterF32 = emitter->counter - emitter->counter2;
    u32 numBehaviorIter = (u32)numBehaviorIterF32;
    f32 numBehaviorIterDelta = numBehaviorIterF32 - numBehaviorIter;
    bool behaviorRepeat = emitter->emissionSpeed > 1.0f;

    PtclInstance* next;

    for (; ptcl != NULL; ptcl = next)
    {
        next = ptcl->next;
        PtclStripe* stripe = ptcl->complexParam->stripe;

        if (!noCalcBehavior)
        {
            if (isStripe && stripe != NULL)
            {
                ptcl->counterS32 = (s32)ptcl->counter;
                if (ptcl->counterS32 >= ptcl->lifespan || ptcl->lifespan == 1 && ptcl->counter != 0.0f)
                {
                    const StripeData* stripeData = reinterpret_cast<const StripeData*>((u32)data + data->stripeDataOffs);

                    if (stripe->queueFront != stripe->queueRear)
                    {
                        if (++stripe->queueFront >= stripeData->numSliceHistory)
                            stripe->queueFront = 0;

                        stripe->queueCount--;

                        stripe->texAnimParam[0].scroll.x += data->texAnimParam[0].texIncScroll.x * emitter->emissionSpeed;
                        stripe->texAnimParam[0].scroll.y += data->texAnimParam[0].texIncScroll.y * emitter->emissionSpeed;
                        stripe->texAnimParam[0].scale.x  += data->texAnimParam[0].texIncScale.x  * emitter->emissionSpeed;
                        stripe->texAnimParam[0].scale.y  += data->texAnimParam[0].texIncScale.y  * emitter->emissionSpeed;
                        stripe->texAnimParam[0].rotate   += data->texAnimParam[0].texIncRotate   * emitter->emissionSpeed;

                        stripe->texAnimParam[1].scroll.x += data->texAnimParam[1].texIncScroll.x * emitter->emissionSpeed;
                        stripe->texAnimParam[1].scroll.y += data->texAnimParam[1].texIncScroll.y * emitter->emissionSpeed;
                        stripe->texAnimParam[1].scale.x  += data->texAnimParam[1].texIncScale.x  * emitter->emissionSpeed;
                        stripe->texAnimParam[1].scale.y  += data->texAnimParam[1].texIncScale.y  * emitter->emissionSpeed;
                        stripe->texAnimParam[1].rotate   += data->texAnimParam[1].texIncRotate   * emitter->emissionSpeed;

                        emitter->numDrawParticle++;

                        stripe->counter++;
                        if (!noMakePtclAttributeBuffer)
                            system->renderers[core]->MakeStripeAttributeBlock(emitter, ptcl);
                    }
                    else
                    {
                        RemoveParticle(ptcl, core);
                    }

                    stripe->counter++;
                    if (!noMakePtclAttributeBuffer)
                        system->renderers[core]->MakeStripeAttributeBlock(emitter, ptcl);

                    continue;
                }
            }
            else
            {
                ptcl->counterS32 = (s32)ptcl->counter;
                if (ptcl->counterS32 >= ptcl->lifespan || ptcl->lifespan == 1 && ptcl->counter > 1.0f)
                {
                    RemoveParticle(ptcl, core);
                    continue;
                }
            }

            if (behaviorRepeat)
            {
                for (u32 i = 0; i < numBehaviorIter; i++)
                    CalcComplexParticleBehavior(emitter, ptcl, 1.0f);

                if (numBehaviorIterDelta != 0.0f)
                    CalcComplexParticleBehavior(emitter, ptcl, numBehaviorIterDelta);
            }
            else
            {
                CalcComplexParticleBehavior(emitter, ptcl, emitter->emissionSpeed);
            }

            if (isStripe && stripe != NULL && emitter->emissionSpeed != 0.0f)
                CalcStripe(emitter, ptcl, stripeData, data, core, noMakePtclAttributeBuffer);


            if (childData != NULL && emitter->emissionSpeed != 0.0f)
                EmitChildParticle(emitter, ptcl, core, childData);

            if (callback1 != NULL)
            {
                ParticleCalcArg arg = {
                    .emitter = emitter,
                    .ptcl = ptcl,
                    .core = core,
                    .noCalcBehavior = noCalcBehavior,
                };
                callback1(arg);

                if (ptcl->data == NULL)
                    continue;
            }
        }

        if (!isStripe && !noMakePtclAttributeBuffer)
        {
            ptcl->ptclAttributeBuffer = ptclAttributeBuffer;
            MakeParticleAttributeBuffer(ptclAttributeBuffer++, ptcl, shaderAvailableAttribFlg);
            emitter->numDrawParticle++;

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

        if (simpleStripe && !noMakePtclAttributeBuffer)
            system->renderers[core]->MakeStripeAttributeBlock(emitter, ptcl);
    }

    if (complexStripe)
        system->renderers[core]->MakeConnectionStripeAttributeBlock(emitter);

    emitter->emitterBehaviorFlg |= EmitterBehaviorFlag_IsCalculated;
    return emitter->numParticles;
}

// This was moved to eft_EmitterChild.cpp
u32 EmitterComplexCalc::CalcChildParticle(EmitterInstance* emitter, CpuCore core, bool noCalcBehavior, bool noMakePtclAttributeBuffer)
{
    emitter->numDrawChildParticle = 0;
    if (emitter->numChildParticles == 0)
        return 0;

    const ComplexEmitterData* data = emitter->GetComplexEmitterData();
    const ChildData* childData = emitter->GetChildData();

    if (!noMakePtclAttributeBuffer)
    {
        Renderer** const renderers = emitter->emitterSet->system->renderers;

        emitter->childPtclAttributeBuffer = static_cast<PtclAttributeBuffer*>(renderers[core]->AllocFromDoubleBuffer(sizeof(PtclAttributeBuffer) * emitter->numChildParticles));
        if (emitter->childPtclAttributeBuffer == NULL)
        {
            emitter->childEmitterDynamicUniformBlock = NULL;
            return 0;
        }

        emitter->childEmitterDynamicUniformBlock = MakeEmitterUniformBlock(emitter, core, childData, false);
        if (emitter->childEmitterDynamicUniformBlock == NULL)
        {
            emitter->childPtclAttributeBuffer = NULL;
            return 0;
        }
    }
    else
    {
        emitter->childPtclAttributeBuffer = NULL;
        emitter->childEmitterDynamicUniformBlock = NULL;
    }

    CustomActionParticleCalcCallback callback1 = mSys->GetCurrentCustomActionParticleCalcCallback(emitter);
    CustomActionParticleMakeAttributeCallback callback2 = mSys->GetCurrentCustomActionParticleMakeAttributeCallback(emitter);

    u32 shaderAvailableAttribFlg = emitter->childShader[ShaderType_Normal]->shaderAvailableAttribFlg;

    f32 numBehaviorIterF32 = emitter->counter - emitter->counter2;
    u32 numBehaviorIter = (u32)numBehaviorIterF32;
    f32 numBehaviorIterDelta = numBehaviorIterF32 - numBehaviorIter;
    bool behaviorRepeat = emitter->emissionSpeed > 1.0f;

    PtclInstance* ptcl = emitter->childParticleHead;
    PtclInstance* next;

    for (; ptcl != NULL; ptcl = next)
    {
        next = ptcl->next;

        if (!noCalcBehavior)
        {
            ptcl->counterS32 = (s32)ptcl->counter;
            if (ptcl->counterS32 >= ptcl->lifespan || ptcl->lifespan == 1 && ptcl->counter > 1.0f)
            {
                RemoveParticle(ptcl, core);
                continue;
            }

            if (data->childFlags & 0x40)
            {
                ptcl->matrixSRT = emitter->matrixSRT;
                ptcl->matrixRT = emitter->matrixRT;
            }

            if (behaviorRepeat)
            {
                for (u32 i = 0; i < numBehaviorIter; i++)
                    CalcChildParticleBehavior(emitter, ptcl, 1.0f);

                if (numBehaviorIterDelta != 0.0f)
                    CalcChildParticleBehavior(emitter, ptcl, numBehaviorIterDelta);
            }
            else
            {
                CalcChildParticleBehavior(emitter, ptcl, emitter->emissionSpeed);
            }
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

            if (ptcl->data == NULL)
                continue;
        }

        if (!noMakePtclAttributeBuffer)
        {
            ptcl->ptclAttributeBuffer = &emitter->childPtclAttributeBuffer[emitter->numDrawChildParticle];
            MakeParticleAttributeBuffer(ptcl->ptclAttributeBuffer, ptcl, shaderAvailableAttribFlg);
            emitter->numDrawParticle++;

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

    emitter->emitterBehaviorFlg |= EmitterBehaviorFlag_IsCalculated;
    return emitter->numDrawChildParticle;
}

} } // namespace nw::eft
