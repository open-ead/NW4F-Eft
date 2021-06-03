#ifndef EFT_TEMPORARY_BUFFER_H_
#define EFT_TEMPORARY_BUFFER_H_

#include <eft_Heap.h>

#define INCLUDE_CAFE
#include <eft_Types.h>

namespace nw { namespace eft {

struct TemporaryBuffer
{
    inline void Initialize(Heap* heap, u32 size);
    inline void Swap();
    inline void FlushCache();

    u32 currentBufferIdx;
    u32 bufferSize;
    u32 bufferUsedSize;
    u32 bufferFlushedSize;
    void* buffer[2];
};
static_assert(sizeof(TemporaryBuffer) == 0x18, "TemporaryBuffer size mismatch");

void TemporaryBuffer::Initialize(Heap* heap, u32 size)
{
    currentBufferIdx = 0;
    bufferSize = size;
    bufferUsedSize = 0;
    bufferFlushedSize = 0;
    buffer[0] = heap->Alloc(bufferSize, 0x100);
    buffer[1] = heap->Alloc(bufferSize, 0x100);
}

void TemporaryBuffer::Swap()
{
    currentBufferIdx = !currentBufferIdx;
    bufferUsedSize = 0;
    bufferFlushedSize = 0;
}

void TemporaryBuffer::FlushCache()
{
    if (bufferFlushedSize != bufferUsedSize)
    {
        DCFlushRange(static_cast<u8*>(buffer[currentBufferIdx]) + bufferFlushedSize, bufferUsedSize - bufferFlushedSize);
        bufferFlushedSize = bufferUsedSize;
    }
}

} } // namespace nw::eft

#endif // EFT_TEMPORARY_BUFFER_H_
