#ifndef EFT_RENDER_CONTEXT_H_
#define EFT_RENDER_CONTEXT_H_

#include <eft_CafeWrapper.h>
#include <eft_Enum.h>

namespace nw { namespace eft {

struct TextureRes;
struct FragmentTextureLocation;
struct VertexTextureLocation;

class RenderContext
{
public:
    RenderContext();

    void SetupCommonState();
    void SetupBlendType(BlendType blendType) const;
    void SetupZBufATest(ZBufATestType zBufATestType) const;
    void SetupDisplaySideType(DisplaySideType displaySideType) const;
    void _SetupFragmentTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, FragmentTextureLocation samplerLocation) const;
    void SetupTexture(const TextureRes* texture, TextureSlot slot, FragmentTextureLocation samplerLocation);
    void SetupTexture(const GX2Texture* texture, TextureSlot slot, FragmentTextureLocation samplerLocation);
    void _SetupFragment2DArrayTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, FragmentTextureLocation samplerLocation) const;
    void SetupUserFragment2DArrayTexture(const GX2Texture* texture, TextureSlot slot, FragmentTextureLocation samplerLocation) const;
    void _SetupVertexTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, VertexTextureLocation samplerLocation) const;
    void SetupUserVertexTexture(const GX2Texture* texture, TextureSlot slot, VertexTextureLocation samplerLocation) const;
    void _SetupVertex2DArrayTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, VertexTextureLocation samplerLocation) const;
    void SetupVertexArrayTexture(const GX2Texture* texture, TextureSlot slot, VertexTextureLocation samplerLocation) const;

    TextureSampler textureSamplers[TextureSlot_Max];
    TextureSampler textureSampler2;
    TextureSampler textureSampler2DArray;
};
static_assert(sizeof(RenderContext) == 0xC0, "RenderContext size mismatch");

} } // namespace nw::eft

#endif // EFT_RENDER_CONTEXT_H_
