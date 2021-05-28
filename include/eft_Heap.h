#ifndef EFT_HEAP_H_
#define EFT_HEAP_H_

namespace nw { namespace eft {

class Heap
{
public:
    Heap() { }
    virtual ~Heap() { }

    virtual void* Alloc(u32 size, s32 alignment = 0x80) = 0;
    virtual void Free(void* ptr) = 0;
};

} } // namespace nw::eft

#endif // EFT_HEAP_H_