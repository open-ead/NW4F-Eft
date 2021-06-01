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
    if (config.GetHeap())
        Initialize(config.GetHeap(), config);
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
    _530 = 0;
    numEmitterSetMaxMask = numEmitterSetMax - 1;
    currentCallbackID = CustomActionCallBackID_Invalid;
    numEmitterMaxMask = numEmitterMax - 1;
    _8A8 = -1;
    numStripeMaxMask = numStripeMax - 1;
    numCalcEmitterSet = 0;
    numCalcEmitter = 0;
    currentEmitterSetIdx = 0;
    _564 = 0;
    _538 = 0;
    currentEmitterIdx = 0;
    currentEmitterSetCreateID = 0;
    numParticleMaxMask = numParticleMax - 1;
    _A20 = 0;
    currentParticleIdx = 0;

    memset(emitterGroups,       0, 64 * sizeof(EmitterInstance*));
    memset(emitterSetGroupHead, 0, 64 * sizeof(EmitterSet*));
    memset(emitterSetGroupTail, 0, 64 * sizeof(EmitterSet*));
    memset(stripeGroups,        0, 64 * sizeof(PtclStripe*));
    memset(_570[CpuCore_0],     0, 64 * sizeof(u32));
    memset(_570[CpuCore_1],     0, 64 * sizeof(u32));
    memset(_570[CpuCore_2],     0, 64 * sizeof(u32));

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
        customActionParticleMakeAttrCallback[i] = NULL;
        customActionEmitterPostCalcCallback[i] = NULL;
        customActionEmitterDrawOverrideCallback[i] = NULL;
    }

    for (u32 i = 0; i < CustomShaderCallBackID_Max; i++)
    {
        customShaderEmitterCalcPostCallback[i] = NULL;
        customShaderDrawOverrideCallback[i] = NULL;
        customShaderRenderStateSetCallback[i] = NULL;
    }

    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        sortedEmitterSets[i] = static_cast<PtclViewZ*>(heap->Alloc(sizeof(PtclViewZ) * numEmitterSetMax));
        numSortedEmitterSets[i] = 0;
        _A14[i] = 0;
    }

    initialized = true;
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
