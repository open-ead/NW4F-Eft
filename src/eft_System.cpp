#include <eft_Config.h>
#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_EmitterStaticUniformBlock.h>
#include <eft_Heap.h>
#include <eft_Particle.h>
#include <eft_Random.h>
#include <eft_Renderer.h>
#include <eft_System.h>

#include <cstring>
#include <new>

namespace nw { namespace eft {

System::System(const Config& config)
    : initialized(false)
{
    if (config.GetHeap())
        Initialize(config.GetHeap(), config);
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
}

} } // namespace nw::eft
