#ifndef EFT_CONFIG_H_
#define EFT_CONFIG_H_

#include <types.h>

namespace nw { namespace eft {

class Heap;

class Config
{
public:
    Config();
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
    u32 numPtclMax;
    u32 numEmitterSetMax;
    u32 numResourceMax;
    u32 numStripeMax;
    u32 _18;
};

} } // namespace nw::eft

#endif // EFT_CONFIG_H_
