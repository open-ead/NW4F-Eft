#ifndef EFT_TEMPORARY_BUFFER_H_
#define EFT_TEMPORARY_BUFFER_H_

#include <eft_Heap.h>

#define INCLUDE_CAFE
#include <eft_Types.h>

namespace nw { namespace eft {

struct TemporaryBuffer
{
    void Initialize(u32 size, bool isTripleBuffer);
    void Swap();
    void* Alloc(u32 size);
    void FlushCache();

    u32 currentBufferIdx;
    u32 bufferSize;
    u32 _unused[3];
    u32 bufferUsedSize[3];
    u32 bufferFlushedSize[3];
    void* buffer[3];
};
static_assert(sizeof(TemporaryBuffer) == 0x38, "TemporaryBuffer size mismatch");

} } // namespace nw::eft

#endif // EFT_TEMPORARY_BUFFER_H_
