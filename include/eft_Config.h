#ifndef EFT_CONFIG_H_
#define EFT_CONFIG_H_

#include <types.h>

namespace nw { namespace eft {

class Heap;

class Config
{
public:
    Config()
        : heap (NULL)
        , numEmitterMax(0x100)
        , numParticleMax(0x800)
        , numEmitterSetMax(0x80)
        , numResourceMax(1)
        , numStripeMax(0x100)
        , _18(0x20000)
    {
    }

    virtual ~Config() { } // deleted

    virtual void SetHeap(Heap* heap) // deleted, not sure if correct
    {
        this->heap = heap;
    }

    virtual Heap* GetHeap() const
    {
        return heap;
    }

    Heap* heap;
    u32 numEmitterMax;
    u32 numParticleMax;
    u32 numEmitterSetMax;
    u32 numResourceMax;
    u32 numStripeMax;
    u32 _18;
};

} } // namespace nw::eft

#endif // EFT_CONFIG_H_
