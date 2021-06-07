#include <eft_Config.h>
#include <eft_Renderer.h>

namespace nw { namespace eft {

void Renderer::SwapDoubleBuffer()
{
    doubleBuffer.Swap();
    stripeNumDrawVertex = 0;
}

void* Renderer::AllocFromDoubleBuffer(u32 size)
{
    if (size == 0)
        return NULL;

    return doubleBuffer.Alloc(size);
}

void Renderer::FlushCache()
{
    doubleBuffer.FlushCache();
}

} } // namespace nw::eft
