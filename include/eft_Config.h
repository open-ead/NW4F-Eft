#ifndef EFT_CONFIG_H_
#define EFT_CONFIG_H_

#include <eft_Types.h>

namespace nw { namespace eft {

class Heap;

class Config
{
public:
    Config(Heap* heap, Heap* dynamicHeap, u32 numEmitterMax, u32 numParticleMax, u32 numEmitterSetMax, u32 numStripeMax,
           u32 numResourceMax, u32 doubleBufferSize, bool suppressOutputLog, bool isTripleBuffer, u32 numExtraResourceMax)
        : heap(heap)
        , dynamicHeap(dynamicHeap)
        , numEmitterMax(numEmitterMax)
        , numParticleMax(numParticleMax)
        , numEmitterSetMax(numEmitterSetMax)
        , numStripeMax(numStripeMax)
        , numResourceMax(numResourceMax)
        , doubleBufferSize(doubleBufferSize)
        , suppressOutputLog(suppressOutputLog)
        , isTripleBuffer(isTripleBuffer)
        , numExtraResourceMax(numExtraResourceMax)
    {
    }

    virtual ~Config() { }

    void SetEffectHeap(Heap* heap)
    {
        this->heap = heap;
    }

    Heap* GetEffectHeap() const
    {
        return heap;
    }

    Heap* heap;
    Heap* dynamicHeap;
    u32 numEmitterMax;
    u32 numParticleMax;
    u32 numEmitterSetMax;
    u32 numStripeMax;
    u32 numResourceMax;
    u32 doubleBufferSize;
    bool suppressOutputLog;
    bool isTripleBuffer;
    u32 numExtraResourceMax; // Used to be fixed to 0x10u in NSMBU in System::Initialize()
};
static_assert(sizeof(Config) == 0x2C, "Config size mismatch");

} } // namespace nw::eft

#endif // EFT_CONFIG_H_
