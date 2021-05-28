#ifndef EFT_RANDOM_H_
#define EFT_RANDOM_H_

#include <types.h>

namespace nw { namespace eft {

class Heap;

class PtclRandom
{
public:
    PtclRandom();

    static void Initialize(Heap* heap);

    u16 randomVec3Idx;
    u16 randomNormVec3Idx;
    u32 val;
};

} } // namespace nw::eft

#endif // EFT_RANDOM_H_
