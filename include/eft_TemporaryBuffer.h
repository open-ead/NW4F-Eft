#ifndef EFT_TEMPORARY_BUFFER_H_
#define EFT_TEMPORARY_BUFFER_H_

#include <eft_Types.h>

namespace nw { namespace eft {

struct TemporaryBuffer
{
    void Initialize(nw::eft::Heap* heap, u32 size)
    {
        currentBufferIdx = 0;
        bufferSize = size;
        bufferUsedSize[0] = 0;
        bufferUsedSize[1] = 0;
        buffer[0] = heap->Alloc(bufferSize, 0x100);
        buffer[1] = heap->Alloc(bufferSize, 0x100);
    }

    u32 currentBufferIdx;
    u32 bufferSize;
    u32 bufferUsedSize[2];
    void* buffer[2];
};
static_assert(sizeof(TemporaryBuffer) == 0x18, "TemporaryBuffer size mismatch");

} } // namespace nw::eft

#endif // EFT_TEMPORARY_BUFFER_H_
