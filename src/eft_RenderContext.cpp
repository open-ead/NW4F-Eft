#include <eft_RenderContext.h>
#include <eft_Shader.h>

namespace nw { namespace eft {

RenderContext::RenderContext()
{
    textureSampler.Setup(TextureFilterMode_Linear, TextureWrapMode_Wrap, TextureWrapMode_Wrap);
    textureSampler2.Setup(TextureFilterMode_Linear, TextureWrapMode_Mirror, TextureWrapMode_Mirror);
}

void RenderContext::SetupCommonState()
{
    GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_DISABLE, GX2_DISABLE);
    GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_ENABLE, GX2_DISABLE, GX2_ENABLE);

    blendType = BlendType_Max;
    zBufATestType = ZBufATestType_Max;
}

void RenderContext::SetupBlendType(BlendType blendType)
{
    switch (blendType)
    {
    case BlendType_Type0:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE_MINUS_SRC_ALPHA,
                           GX2_BLEND_COMBINE_ADD,
                           GX2_DISABLE,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE_MINUS_SRC_ALPHA,
                           GX2_BLEND_COMBINE_ADD);
        break;
    case BlendType_Type1:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD,
                           GX2_DISABLE,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD);
        break;
    case BlendType_Type2:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_DST_MINUS_SRC,
                           GX2_DISABLE,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_DST_MINUS_SRC);
        break;
    case BlendType_Type3:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_ONE_MINUS_DST_COLOR,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD,
                           GX2_DISABLE,
                           GX2_BLEND_ONE_MINUS_DST_COLOR,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD);
        break;
    case BlendType_Type4:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_ZERO,
                           GX2_BLEND_SRC_COLOR ,
                           GX2_BLEND_COMBINE_ADD,
                           GX2_DISABLE,
                           GX2_BLEND_ONE_MINUS_DST_COLOR,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD);
        break;
    }

    this->blendType = blendType;
}

void RenderContext::SetupZBufATest(ZBufATestType zBufATestType)
{
    switch (zBufATestType)
    {
    case ZBufATestType_Type0:
        GX2SetDepthOnlyControl(GX2_TRUE, GX2_FALSE, GX2_COMPARE_LEQUAL);
        GX2SetAlphaTest(GX2_TRUE, GX2_COMPARE_GREATER, 0.0f);
        GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_ENABLE, GX2_DISABLE, GX2_ENABLE);
        break;
    case ZBufATestType_Type1:
        GX2SetDepthOnlyControl(GX2_FALSE, GX2_FALSE, GX2_COMPARE_LEQUAL);
        GX2SetAlphaTest(GX2_TRUE, GX2_COMPARE_GREATER, 0.0f);
        GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_ENABLE, GX2_DISABLE, GX2_ENABLE);
        break;
    case ZBufATestType_Type2:
        GX2SetDepthOnlyControl(GX2_TRUE, GX2_TRUE, GX2_COMPARE_LEQUAL);
        GX2SetAlphaTest(GX2_TRUE, GX2_COMPARE_GREATER, 0.5f);
        GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_DISABLE, GX2_DISABLE, GX2_ENABLE);
        break;
    }

    this->zBufATestType = zBufATestType;
}

void RenderContext::SetupDisplaySideType(DisplaySideType displaySideType) const
{
    switch (displaySideType)
    {
    case DisplaySideType_Cull_None:
        GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_FALSE, GX2_FALSE);
        break;
    case DisplaySideType_Cull_Back:
        GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_FALSE, GX2_TRUE);
        break;
    case DisplaySideType_Cull_Front:
        GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_TRUE, GX2_FALSE);
        break;
    }
}

void RenderContext::_SetupFragmentTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, FragmentTextureLocation samplerLocation) const
{
    GX2SetPixelTexture(texture, samplerLocation.location);
    GX2SetPixelSampler(sampler, samplerLocation.location);
}

void RenderContext::SetupTexture(const TextureRes* texture, TextureSlot slot, FragmentTextureLocation samplerLocation)
{
    if (texture == NULL || samplerLocation.location == 0xFFFFFFFF)
        return;

    GX2SetPixelTexture(&texture->gx2Texture, samplerLocation.location);

    textureSampler.Setup(static_cast<TextureFilterMode>(texture->filterMode),
                         static_cast<TextureWrapMode>(texture->wrapMode & 0xF),
                         static_cast<TextureWrapMode>(texture->wrapMode >> 4));
    textureSampler.SetupLOD(texture->maxLOD, texture->biasLOD);

    GX2SetPixelSampler(&textureSampler.sampler, samplerLocation.location);
}

void RenderContext::SetupTexture(const GX2Texture* texture, TextureSlot slot, FragmentTextureLocation samplerLocation)
{
    _SetupFragmentTexture(texture, &textureSampler2.sampler, slot, samplerLocation);
}

} } // namespace nw::eft
