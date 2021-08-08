#include <eft_Misc.h>
#include <eft_StreamOutBuffer.h>

namespace nw { namespace eft {

void StreamOutAttributeBuffer::Invalidate()
{
    data[0] = NULL;
    data[1] = NULL;
    memset(&buffer[0], 0, sizeof(GX2StreamOutBuffer));
    memset(&context[0], 0, sizeof(GX2StreamOutContext));
    memset(&buffer[1], 0, sizeof(GX2StreamOutBuffer));
    memset(&context[1], 0, sizeof(GX2StreamOutContext));
}

void StreamOutAttributeBuffer::Initialize(u32 size)
{
    dataSize = size;

    u32 totalSize = (dataSize + sizeof(GX2StreamOutContext)) + (0x20 - 1) & ~(0x20 - 1);
    data[0] = static_cast<math::VEC4*>(AllocFromDynamicHeap(totalSize, GX2_STREAMOUT_BUFFER_ALIGNMENT));
    data[1] = static_cast<math::VEC4*>(AllocFromDynamicHeap(totalSize, GX2_STREAMOUT_BUFFER_ALIGNMENT));

    if (data[0] != NULL && data[1] != NULL)
    {
        for (u32 i = 0; i < size / sizeof(math::VEC4); i++)
        {
            data[0][i] = (math::VEC4){ 0.0f, 0.0f, 0.0f, 0.0f };
            data[1][i] = (math::VEC4){ 0.0f, 0.0f, 0.0f, 0.0f };
        }

        memset(&buffer[0], 0, sizeof(GX2StreamOutBuffer));
        memset(&context[0], 0, sizeof(GX2StreamOutContext));
        memset(&buffer[1], 0, sizeof(GX2StreamOutBuffer));
        memset(&context[1], 0, sizeof(GX2StreamOutContext));

        buffer[0].size = dataSize;
        buffer[0].dataPtr = data[0];
        buffer[0].ctxPtr = reinterpret_cast<GX2StreamOutContext*>((u32)data[0] + dataSize);

        buffer[1].size = dataSize;
        buffer[1].dataPtr = data[1];
        buffer[1].ctxPtr = reinterpret_cast<GX2StreamOutContext*>((u32)data[1] + dataSize);


        GX2Invalidate(GX2_INVALIDATE_CPU, buffer[0].dataPtr, totalSize);
        GX2Invalidate(GX2_INVALIDATE_CPU, buffer[1].dataPtr, totalSize);

        buffer[0].vertexStride = sizeof(math::VEC4);
        buffer[1].vertexStride = sizeof(math::VEC4);
    }
    else
    {
        if (data[0] != NULL)
        {
            FreeFromDynamicHeap(data[0], true);
            data[0] = NULL;
        }

        if (data[1] != NULL)
        {
            FreeFromDynamicHeap(data[1], true);
            data[1] = NULL;
        }

        Invalidate();
    }
}

void StreamOutAttributeBuffer::Finalize()
{
    if (data[0] != NULL)
    {
        FreeFromDynamicHeap(data[0], false);
        data[0] = NULL;
    }

    if (data[1] != NULL)
    {
        FreeFromDynamicHeap(data[1], false);
        data[1] = NULL;
    }
}

bool StreamOutAttributeBuffer::Bind(u32 buffer, u32 target, bool swap, bool apply)
{
    if (data[0] != NULL && data[1] != NULL)
    {
        GX2SetAttribBuffer(buffer, this->buffer[swap].size, sizeof(nw::math::VEC4), data[swap]);

        currentBufferIdx = swap ^ 1;

        if (apply)
        {
            GX2SetStreamOutBuffer(target, &this->buffer[currentBufferIdx]);
            GX2SetStreamOutContext(target, &this->buffer[currentBufferIdx], GX2_TRUE);
        }

        return true;
    }

    return false;
}

void StreamOutAttributeBuffer::UnBind(u32 target)
{
    if (data[0] != NULL && data[1] != NULL)
    {
        GX2SaveStreamOutContext(target, &buffer[currentBufferIdx]);
        GX2Invalidate(static_cast<GX2InvalidateType>(GX2_INVALIDATE_ATTRIB_BUFFER
                                                     | GX2_INVALIDATE_STREAMOUT_BUFFER),
                      buffer[currentBufferIdx].dataPtr, dataSize);
    }
}

} } // namespace nw::eft
