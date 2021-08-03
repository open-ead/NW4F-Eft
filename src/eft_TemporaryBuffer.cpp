#include <eft_Misc.h>
#include <eft_TemporaryBuffer.h>

namespace nw { namespace eft {

void TemporaryBuffer::Initialize(u32 size, bool isTripleBuffer)
{
    currentBufferIdx = 0;
    bufferSize = size;
    _unused[0] = 0;
    _unused[1] = 0;
    _unused[2] = 0;
    bufferUsedSize[0] = 0;
    bufferUsedSize[1] = 0;
    bufferUsedSize[2] = 0;
    bufferFlushedSize[0] = 0;
    bufferFlushedSize[1] = 0;
    bufferFlushedSize[2] = 0;
    buffer[0] = AllocFromStaticHeap(bufferSize, 0x100);
    buffer[1] = AllocFromStaticHeap(bufferSize, 0x100);
    buffer[2] = isTripleBuffer ? AllocFromStaticHeap(bufferSize, 0x100) : NULL;
}
void TemporaryBuffer::Swap()
{
    bufferUsedSize[currentBufferIdx] = 0;
    bufferFlushedSize[currentBufferIdx] = 0;
    _unused[currentBufferIdx] = 0;

    if (buffer[2] != NULL)
    {
        if (currentBufferIdx == 2)
            currentBufferIdx = 0;

        else
            currentBufferIdx++;
    }
    else
    {
        currentBufferIdx = 1 - currentBufferIdx;
    }

    bufferUsedSize[currentBufferIdx] = 0;
    bufferFlushedSize[currentBufferIdx] = 0;
    _unused[currentBufferIdx] = 0;
    bufferUsedSize[currentBufferIdx] = 0; // ??
}

void* TemporaryBuffer::Alloc(u32 size)
{
    if (size == 0)
        return NULL;

    u32 sizeAligned = size + (0x100 - 1) & ~(0x100 - 1);
    if (bufferUsedSize[currentBufferIdx] + sizeAligned > bufferSize)
    {
        WARNING("DoubleBuffer is Empty.\n");
        return NULL;
    }

    u8* ret = static_cast<u8*>(buffer[currentBufferIdx]) + bufferUsedSize[currentBufferIdx];
    bufferUsedSize[currentBufferIdx] += sizeAligned;

    DCZeroRange(ret, sizeAligned);

    return ret;
}

void TemporaryBuffer::FlushCache()
{
    if (bufferFlushedSize[currentBufferIdx] != bufferUsedSize[currentBufferIdx])
    {
        DCFlushRange(static_cast<u8*>(buffer[currentBufferIdx]) + bufferFlushedSize[currentBufferIdx], bufferUsedSize[currentBufferIdx] - bufferFlushedSize[currentBufferIdx]);
        bufferFlushedSize[currentBufferIdx] = bufferUsedSize[currentBufferIdx];
    }
}

} } // namespace nw::eft
