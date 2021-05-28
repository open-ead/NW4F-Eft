#include <eft_Config.h>
#include <eft_Heap.h>
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

    numCalcPtcl = 0;
    numUnusedEmitters = numEmitterMax;
    _530 = 0;
    _18 = numEmitterSetMax - 1;
    currentCallbackID = CustomActionCallBackID_Invalid;
    _548 = numEmitterMax - 1;
    _8A8 = -1;
    _550 = numStripeMax - 1;
    numCalcEmitterSet = 0;
    _554 = 0;
    _534 = 0;
    _564 = 0;
    _538 = 0;
    _3E4 = 0;
    numCreatedEmitterSet = 0;
    _54C = numParticleMax - 1;
    _A20 = 0;
    currentPtclIdx = 0;

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

    // ...
}

} } // namespace nw::eft
