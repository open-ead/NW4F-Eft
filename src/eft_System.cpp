#include <eft_Config.h>
#include <eft_EmitterComplex.h>
#include <eft_EmitterSet.h>
#include <eft_Handle.h>
#include <eft_Heap.h>
#include <eft_Particle.h>
#include <eft_Random.h>
#include <eft_Renderer.h>
#include <eft_System.h>
#include <eft_UniformBlock.h>

#include <cstring>
#include <new>

namespace nw { namespace eft {

System::System(const Config& config)
    : initialized(false)
{
    if (config.GetEffectHeap())
        Initialize(config.GetEffectHeap(), config);
}

System::~System()
{
}

void System::Initialize(Heap* argHeap, const Config& config)
{
    heap = argHeap;
    numResourceMax = config.numResourceMax;
    numEmitterMax = config.numEmitterMax;
    numParticleMax = config.numParticleMax;
    numEmitterSetMax = config.numEmitterSetMax;
    numStripeMax = config.numStripeMax;

    numCalcParticle = 0;
    numUnusedEmitters = numEmitterMax;
    _unused0 = 0;
    numEmitterSetMaxMask = numEmitterSetMax - 1;
    currentCallbackID = CustomActionCallBackID_Invalid;
    numEmitterMaxMask = numEmitterMax - 1;
    _unkCallbackVal = -1;
    numStripeMaxMask = numStripeMax - 1;
    numCalcEmitterSet = 0;
    numCalcEmitter = 0;
    currentEmitterSetIdx = 0;
    numCalcStripe = 0;
    currentStripeIdx = 0;
    currentEmitterIdx = 0;
    currentEmitterSetCreateID = 0;
    numParticleMaxMask = numParticleMax - 1;
    doubleBufferSwapped = 0;
    currentParticleIdx = 0;

    memset(emitterGroups,           0, 64 * sizeof(EmitterInstance*));
    memset(emitterSetGroupHead,     0, 64 * sizeof(EmitterSet*));
    memset(emitterSetGroupTail,     0, 64 * sizeof(EmitterSet*));
    memset(stripeGroups,            0, 64 * sizeof(PtclStripe*));
    memset(_unusedFlags[CpuCore_0], 0, 64 * sizeof(u32));
    memset(_unusedFlags[CpuCore_1], 0, 64 * sizeof(u32));
    memset(_unusedFlags[CpuCore_2], 0, 64 * sizeof(u32));

    PtclRandom::Initialize(heap);

    resourceWork = heap->Alloc(sizeof(Resource*) * (numResourceMax + 0x10u));
    resources = new (resourceWork) Resource*[(numResourceMax + 0x10u)];
    memset(resources, 0, sizeof(Resource*) * (numResourceMax + 0x10u));

    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        rendererWork[i] = heap->Alloc(sizeof(Renderer));
        renderers[i] = new (rendererWork[i]) Renderer(heap, this, config);
    }

    emitterSetWork = heap->Alloc(sizeof(EmitterSet) * numEmitterSetMax + 32);
    emitterSets = new (emitterSetWork) EmitterSet[numEmitterSetMax];

    for (s32 i = 0; i < numEmitterSetMax; i++)
        emitterSets[i].system = this;

    emitters = static_cast<EmitterInstance*>(heap->Alloc(sizeof(EmitterInstance) * numEmitterMax));
    emitterStaticUniformBlocks = static_cast<EmitterStaticUniformBlock*>(heap->Alloc(sizeof(EmitterStaticUniformBlock) * numEmitterMax * 2));

    for (s32 i = 0; i < numEmitterMax; i++)
    {
        emitters[i].calc = NULL;
        emitters[i].emitterStaticUniformBlock = &emitterStaticUniformBlocks[i];
        emitters[i].childEmitterStaticUniformBlock = &emitterStaticUniformBlocks[numEmitterMax + i];
    }

    particles = static_cast<PtclInstance*>(heap->Alloc(sizeof(PtclInstance) * numParticleMax));
    alphaAnim = static_cast<AlphaAnim*>(heap->Alloc(sizeof(AlphaAnim) * numParticleMax));
    scaleAnim = static_cast<ScaleAnim*>(heap->Alloc(sizeof(ScaleAnim) * numParticleMax));

    for (s32 i = 0; i < numParticleMax; i++)
    {
        particles[i].data = NULL;
        particles[i].alphaAnim = &alphaAnim[i];
        particles[i].scaleAnim = &scaleAnim[i];
    }

    emitterSimpleCalcWork = heap->Alloc(sizeof(EmitterSimpleCalc));
    emitterCalc[EmitterType_Simple] = new (emitterSimpleCalcWork) EmitterSimpleCalc(this);

    emitterComplexCalcWork = heap->Alloc(sizeof(EmitterComplexCalc));
    emitterCalc[EmitterType_Complex] = new (emitterComplexCalcWork) EmitterComplexCalc(this);

    EmitterCalc::InitializeFluctuationTable(heap);

    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        particlesToRemove[i] = static_cast<PtclInstance**>(heap->Alloc(sizeof(PtclInstance*) * numParticleMax));
        childParticles[i] = static_cast<PtclInstance**>(heap->Alloc(sizeof(PtclInstance*) * numParticleMax));
        numParticleToRemove[i] = 0;
        numChildParticle[i] = 0;

        for (s32 j = 0; j < numParticleMax; j++)
        {
            particlesToRemove[i][j] = NULL;
            childParticles[i][j] = NULL;
        }
    }

    stripes = static_cast<PtclStripe*>(heap->Alloc(sizeof(PtclStripe) * numStripeMax));

    for (s32 i = 0; i < numStripeMax; i++)
    {
        stripes[i].data = NULL;
        stripes[i].queueFront = 0;
        stripes[i].queueRear = 0;
    }

    for (u32 i = 0; i < CustomActionCallBackID_Max; i++)
    {
        customActionEmitterPreCalcCallback[i] = NULL;
        customActionParticleEmitCallback[i] = NULL;
        customActionParticleRemoveCallback[i] = NULL;
        customActionParticleCalcCallback[i] = NULL;
        customActionParticleMakeAttributeCallback[i] = NULL;
        customActionEmitterPostCalcCallback[i] = NULL;
        customActionEmitterDrawOverrideCallback[i] = NULL;
    }

    for (u32 i = 0; i < CustomShaderCallBackID_Max; i++)
    {
        customShaderEmitterPostCalcCallback[i] = NULL;
        customShaderDrawOverrideCallback[i] = NULL;
        customShaderRenderStateSetCallback[i] = NULL;
    }

    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        sortedEmitterSets[i] = static_cast<PtclViewZ*>(heap->Alloc(sizeof(PtclViewZ) * numEmitterSetMax));
        numSortedEmitterSets[i] = 0;
        _unused1[i] = 0;
    }

    initialized = true;
}

void System::RemoveStripe(PtclStripe* stripe)
{
    stripe->data = NULL;

    if (stripeGroups[stripe->groupID] == stripe)
    {
        stripeGroups[stripe->groupID] = stripe->next;

        if (stripeGroups[stripe->groupID] != NULL)
            stripeGroups[stripe->groupID]->prev = NULL;
    }
    else
    {
        if (stripe->next != NULL)
            stripe->next->prev = stripe->prev;

        //if (stripe->prev != NULL) <-- No check, because... Nintendo
            stripe->prev->next = stripe->next;
    }
}

void System::RemovePtcl_()
{
    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        for (s32 j = 0; j < numParticleToRemove[i]; j++)
        {
            PtclInstance* ptcl = particlesToRemove[i][j];

            // EmitterCalc::RemoveParticle(PtclInstance*, CpuCore)
            {
                EmitterInstance* emitter = ptcl->emitter;

                if (ptcl->type == PtclType_Child) emitter->numChildParticles--;
                else                              emitter->numParticles--;

                if (emitter->particleHead == ptcl)
                {
                    emitter->particleHead = ptcl->next;

                    if (emitter->particleHead != NULL)
                        emitter->particleHead->prev = NULL;

                    if (emitter->particleTail == ptcl)
                        emitter->particleTail = NULL;
                }
                else if (emitter->childParticleHead == ptcl)
                {
                    emitter->childParticleHead = ptcl->next;

                    if (emitter->childParticleHead != NULL)
                        emitter->childParticleHead->prev = NULL;

                    if (emitter->childParticleTail == ptcl)
                        emitter->childParticleTail = NULL;
                }
                else if (emitter->particleTail == ptcl)
                {
                    emitter->particleTail = ptcl->prev;

                    if (emitter->particleTail != NULL)
                        emitter->particleTail->next = NULL;
                }
                else if (emitter->childParticleTail == ptcl)
                {
                    emitter->childParticleTail = ptcl->prev;

                    if (emitter->childParticleTail != NULL)
                        emitter->childParticleTail->next = NULL;
                }
                else
                {
                    if (ptcl->next != NULL)
                        ptcl->next->prev = ptcl->prev;

                    //if (ptcl->prev != NULL) <-- No check, because... Nintendo
                        ptcl->prev->next = ptcl->next;
                }

                CustomActionParticleRemoveCallback callback = GetCurrentCustomActionParticleRemoveCallback( ptcl->emitter );
                if(callback != NULL)
                {
                    ParticleRemoveArg arg = { .ptcl = ptcl };
                    callback(arg);
                }

                ptcl->data = NULL;
            }

            if (ptcl->stripe != NULL)
            {
                RemoveStripe(ptcl->stripe);
                ptcl->stripe = NULL;
            }

            particlesToRemove[i][j] = NULL;
        }

        numParticleToRemove[i] = 0;
    }
}

EmitterSet* System::RemoveEmitterSetFromDrawList(EmitterSet* emitterSet)
{
    EmitterSet* next = emitterSet->next;

    if (emitterSet == emitterSetGroupHead[emitterSet->groupID])
    {
        emitterSetGroupHead[emitterSet->groupID] = emitterSet->next;

        if (emitterSetGroupHead[emitterSet->groupID] != NULL)
            emitterSetGroupHead[emitterSet->groupID]->prev = NULL;

        if (emitterSet == emitterSetGroupTail[emitterSet->groupID])
            emitterSetGroupTail[emitterSet->groupID] = NULL;
    }
    else
    {
        if (emitterSet == emitterSetGroupTail[emitterSet->groupID])
            emitterSetGroupTail[emitterSet->groupID] = emitterSet->prev;

        if (emitterSet->next != NULL)
            emitterSet->next->prev = emitterSet->prev;

        if (emitterSet->prev != NULL)
            emitterSet->prev->next = emitterSet->next;
    }

    emitterSet->next = NULL;
    emitterSet->prev = NULL;

    return next;
}

void System::RemovePtcl()
{
    RemovePtcl_();

    for(u32 i = 0; i < 64u; i++)
        for (EmitterSet* emitterSet = emitterSetGroupHead[i]; emitterSet != NULL; )
            emitterSet = (emitterSet->numEmitter == 0) ? RemoveEmitterSetFromDrawList(emitterSet)
                                                       : emitterSet->next;
}

void System::AddPtclRemoveList(PtclInstance* ptcl, CpuCore core)
{
    particlesToRemove[core][numParticleToRemove[core]++] = ptcl;
    ptcl->lifespan = 0;
}

void System::EmitChildParticle()
{
    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        for (s32 j = 0; j < numChildParticle[i]; j++)
        {
            PtclInstance* ptcl = childParticles[i][j];
            EmitterComplexCalc::EmitChildParticle(ptcl->emitter, ptcl);
            childParticles[i][j] = NULL;
        }

        numChildParticle[i] = 0;
    }
}

void System::AddPtclAdditionList(PtclInstance* ptcl, CpuCore core)
{
    if (numChildParticle[core] > numParticleMax)
        return;

    childParticles[core][numChildParticle[core]] = ptcl;
    numChildParticle[core]++;
}

PtclStripe* System::AllocAndConnectStripe(EmitterInstance* emitter, PtclInstance* ptcl)
{
    s32 i = 0;
    do
    {
        currentStripeIdx++;
        currentStripeIdx &= numStripeMaxMask;

        if (stripes[currentStripeIdx].data == NULL)
        {
            PtclStripe* stripe = &stripes[currentStripeIdx];
            u8 groupID = emitter->groupID;

            if (stripeGroups[groupID] == NULL)
            {
                stripeGroups[groupID] = stripe;
                stripe->next = NULL;
                stripe->prev = NULL;
            }
            else
            {
                stripeGroups[groupID]->prev = stripe;
                stripe->next = stripeGroups[groupID];
                stripeGroups[groupID] = stripe;
                stripe->prev = NULL;
            }

            stripe->particle = ptcl;
            stripe->queueFront = 0;
            stripe->queueRear  = 0;
            stripe->queueCount = 0;
            stripe->groupID = emitter->groupID;
            stripe->data = static_cast<const ComplexEmitterData*>(emitter->data);
            stripe->counter = 0;
            stripe->queue[0].outer = math::VEC3::Zero();

            return stripe;
        }
    } while (++i < numStripeMax);

    return NULL;
}

PtclInstance* System::AllocPtcl(PtclType type)
{
    s32 i = 0;
    do
    {
        currentParticleIdx++;
        currentParticleIdx &= numParticleMaxMask;

        if (particles[currentParticleIdx].data == NULL)
        {
            numEmittedParticle++;
            PtclInstance* ptcl = &particles[currentParticleIdx];
            ptcl->type = type;
            return ptcl;
        }
    } while (++i < numParticleMax);

    return NULL;
}

EmitterSet* System::AllocEmitterSet(Handle* handle)
{
    EmitterSet* emitterSet = NULL;
    s32 i = 0;
    do
    {
        currentEmitterSetIdx++;
        currentEmitterSetIdx &= numEmitterSetMaxMask;

        if (emitterSets[currentEmitterSetIdx].numEmitter == 0)
        {
            emitterSet = &emitterSets[currentEmitterSetIdx];
            break;
        }
    } while (++i < numEmitterSetMax);

    handle->emitterSet = emitterSet;
    return emitterSet;
}

EmitterInstance* System::AllocEmitter(u8 groupID)
{
    EmitterInstance* emitter = NULL;
    s32 i = 0;
    do
    {
        currentEmitterIdx++;
        currentEmitterIdx &= numEmitterMaxMask;

        if (emitters[currentEmitterIdx].calc == NULL)
        {
            emitter = &emitters[currentEmitterIdx];
            break;
        }
    } while (++i < numEmitterMax);

    if (emitter == NULL)
        return NULL;

    if (emitterGroups[groupID] == NULL)
    {
        emitterGroups[groupID] = emitter;
        emitter->next = NULL;
        emitter->prev = NULL;
    }
    else
    {
        emitterGroups[groupID]->prev = emitter;
        emitter->next = emitterGroups[groupID];
        emitterGroups[groupID] = emitter;
        emitter->prev = NULL;
    }

    numUnusedEmitters--;
    return emitter;
}

void System::AddEmitterSetToDrawList(EmitterSet* emitterSet, u8 groupID)
{
    if(emitterSetGroupHead[groupID] == NULL)
    {
        emitterSetGroupHead[groupID] = emitterSet;
        emitterSet->prev = NULL;
        emitterSet->next = NULL;
    }
    else
    {
        emitterSetGroupTail[groupID]->next = emitterSet;
        emitterSet->prev = emitterSetGroupTail[groupID];
        emitterSet->next = NULL;
    }

    emitterSetGroupTail[groupID] = emitterSet;
}

} } // namespace nw::eft
