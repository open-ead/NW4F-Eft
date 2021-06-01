#include <eft_RenderContext.h>

namespace nw { namespace eft {

RenderContext::RenderContext()
{
    textureSampler.Setup(TextureFilterMode_Linear, TextureWrapMode_Wrap, TextureWrapMode_Wrap);
    textureSampler2.Setup(TextureFilterMode_Linear, TextureWrapMode_Mirror, TextureWrapMode_Mirror);
}

} } // namespace nw::eft
