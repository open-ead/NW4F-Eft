#ifndef EFT_RENDER_CONTEXT_H_
#define EFT_RENDER_CONTEXT_H_

#include <eft_CafeWrapper.h>
#include <eft_Enum.h>

namespace nw { namespace eft {

class RenderContext
{
public:
    RenderContext();

    TextureSampler textureSampler;
    TextureSampler textureSampler2;
    BlendType blendType;
    ZBufATestType zBufATestType;
};
static_assert(sizeof(RenderContext) == 0x20, "RenderContext size mismatch");

} } // namespace nw::eft

#endif // EFT_RENDER_CONTEXT_H_
