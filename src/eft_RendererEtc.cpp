#include <eft_Config.h>
#include <eft_Renderer.h>

namespace nw { namespace eft {

void Renderer::SwapDoubleBuffer()
{
    doubleBuffer.Swap();
    stripeNumDrawVertex = 0;
}

void Renderer::FlushCache()
{
    doubleBuffer.FlushCache();
}

} } // namespace nw::eft
