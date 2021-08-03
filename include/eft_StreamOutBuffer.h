#ifndef EFT_STREAM_OUT_BUFFER_H_
#define EFT_STREAM_OUT_BUFFER_H_

#include <math/math_VEC4.h>

#define INCLUDE_CAFE
#include <eft_Types.h>

namespace nw { namespace eft {

struct StreamOutAttributeBuffer
{
    void Invalidate();
    void Initialize(u32 size);
    void Finalize();
    bool Bind(u32 buffer, u32 target, bool swap, bool apply);

    math::VEC4* data[2];
    GX2StreamOutBuffer buffer[2];
    GX2StreamOutContext context[2];
    u32 dataSize;
    u8 currentBufferIdx;
};
static_assert(sizeof(StreamOutAttributeBuffer) == 0x58, "StreamOutAttributeBuffer size mismatch");

} } // namespace nw::eft

#endif // EFT_STREAM_OUT_BUFFER_H_

