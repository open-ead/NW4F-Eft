#include <eft_RenderContext.h>
#include <eft_Shader.h>

namespace nw { namespace eft {

RenderContext::RenderContext()
{
    for (u32 i = 0; i < TextureSlot_Max; i++)
        textureSamplers[i].Initialize(TextureFilterMode_Linear, TextureWrapMode_Wrap, TextureWrapMode_Wrap);

    textureSampler2.Initialize(TextureFilterMode_Linear, TextureWrapMode_Mirror, TextureWrapMode_Mirror);
    textureSampler2DArray.Initialize(TextureFilterMode_Linear, TextureWrapMode_Wrap, TextureWrapMode_Wrap);
}

void RenderContext::SetupCommonState()
{
    GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_ENABLE, GX2_DISABLE, GX2_ENABLE);
    GX2SetDepthStencilControl(GX2_TRUE, GX2_TRUE, GX2_COMPARE_LESS, GX2_FALSE, GX2_FALSE, GX2_COMPARE_ALWAYS, GX2_STENCIL_REPLACE, GX2_STENCIL_REPLACE, GX2_STENCIL_REPLACE, GX2_COMPARE_ALWAYS, GX2_STENCIL_REPLACE, GX2_STENCIL_REPLACE, GX2_STENCIL_REPLACE);
    GX2SetPolygonControl(GX2_FRONT_FACE_CCW, GX2_DISABLE, GX2_DISABLE, GX2_DISABLE, GX2_POLYGON_MODE_TRIANGLE, GX2_POLYGON_MODE_TRIANGLE, GX2_DISABLE, GX2_DISABLE, GX2_DISABLE);
    GX2SetPolygonOffset(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    GX2SetPointSize(1.0f, 1.0f);
    GX2SetPointLimits(1.0f, 1.0f);
    GX2SetLineWidth(1.0f);
    GX2SetPrimitiveRestartIndex(0xFFFFFFFF);
    GX2SetAlphaTest(GX2_DISABLE, GX2_COMPARE_LESS, 0.0f);
    GX2SetAlphaToMask(GX2_FALSE, GX2_ALPHA_TO_MASK_0);
    GX2SetBlendConstantColor(0.0f, 0.0f, 0.0f, 0.0f);
    GX2SetStreamOutEnable(GX2_FALSE);
    GX2SetTessellation(GX2_TESSELLATION_MODE_DISCRETE, GX2_PRIMITIVE_TESSELLATE_TRIANGLES, GX2_INDEX_FORMAT_U32);
    GX2SetMaxTessellationLevel(1.0f);
    GX2SetMinTessellationLevel(1.0f);
}

void RenderContext::SetupBlendType(BlendType blendType) const
{
    switch (blendType)
    {
    case BlendType_Normal:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE_MINUS_SRC_ALPHA,
                           GX2_BLEND_COMBINE_ADD,
                           GX2_DISABLE,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE_MINUS_SRC_ALPHA,
                           GX2_BLEND_COMBINE_ADD);
        break;
    case BlendType_Add:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD,
                           GX2_DISABLE,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD);
        break;
    case BlendType_Sub:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_DST_MINUS_SRC,
                           GX2_DISABLE,
                           GX2_BLEND_SRC_ALPHA,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_DST_MINUS_SRC);
        break;
    case BlendType_Screen:
        GX2SetBlendControl(GX2_RENDER_TARGET_0,
                           GX2_BLEND_ONE_MINUS_DST_COLOR,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD,
                           GX2_DISABLE,
                           GX2_BLEND_ONE_MINUS_DST_COLOR,
                           GX2_BLEND_ONE,
                           GX2_BLEND_COMBINE_ADD);
        break;
    case BlendType_Mult:
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
}

void RenderContext::SetupZBufATest(ZBufATestType zBufATestType) const
{
    switch (zBufATestType)
    {
    case ZBufATestType_Normal:
        GX2SetDepthOnlyControl(GX2_TRUE, GX2_FALSE, GX2_COMPARE_LEQUAL);
        GX2SetAlphaTest(GX2_TRUE, GX2_COMPARE_GREATER, 0.0f);
        GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_ENABLE, GX2_DISABLE, GX2_ENABLE);
        break;
    case ZBufATestType_Ignore_Z:
        GX2SetDepthOnlyControl(GX2_FALSE, GX2_FALSE, GX2_COMPARE_LEQUAL);
        GX2SetAlphaTest(GX2_TRUE, GX2_COMPARE_GREATER, 0.0f);
        GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_ENABLE, GX2_DISABLE, GX2_ENABLE);
        break;
    case ZBufATestType_Alpha:
        GX2SetDepthOnlyControl(GX2_TRUE, GX2_TRUE, GX2_COMPARE_LEQUAL);
        GX2SetAlphaTest(GX2_TRUE, GX2_COMPARE_GREATER, 0.5f);
        GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_DISABLE, GX2_DISABLE, GX2_ENABLE);
        break;
    case ZBufATestType_Opaque:
        GX2SetDepthOnlyControl(GX2_TRUE, GX2_TRUE, GX2_COMPARE_LEQUAL);
        GX2SetAlphaTest(GX2_FALSE, GX2_COMPARE_GREATER, 0.5f);
        GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_DISABLE, GX2_DISABLE, GX2_ENABLE);
        break;
    }
}

void RenderContext::SetupDisplaySideType(DisplaySideType displaySideType) const
{
    switch (displaySideType)
    {
    case DisplaySideType_Both:
        GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_FALSE, GX2_FALSE);
        break;
    case DisplaySideType_Front:
        GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_FALSE, GX2_TRUE);
        break;
    case DisplaySideType_Back:
        GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_TRUE, GX2_FALSE);
        break;
    }
}

void RenderContext::_SetupFragmentTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, FragmentTextureLocation samplerLocation) const
{
    if (texture == NULL || samplerLocation.location == 0xFFFFFFFF)
        return;

    GX2SetPixelTexture(texture, samplerLocation.location);
    GX2SetPixelSampler(sampler, samplerLocation.location);
}

void RenderContext::SetupTexture(const TextureRes* texture, TextureSlot slot, FragmentTextureLocation samplerLocation)
{
    textureSamplers[slot].Setup(static_cast<TextureFilterMode>(texture->filterMode),
                         static_cast<TextureWrapMode>(texture->wrapMode & 0xF),
                         static_cast<TextureWrapMode>(texture->wrapMode >> 4));
    textureSamplers[slot].SetupLOD(texture->maxLOD, texture->biasLOD);

    _SetupFragmentTexture(&texture->gx2Texture, &textureSamplers[slot].sampler, slot, samplerLocation);
}

void RenderContext::SetupTexture(const GX2Texture* texture, TextureSlot slot, FragmentTextureLocation samplerLocation)
{
    _SetupFragmentTexture(texture, &textureSampler2.sampler, slot, samplerLocation);
}

void RenderContext::_SetupFragment2DArrayTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, FragmentTextureLocation samplerLocation) const
{
    _SetupFragmentTexture(texture, sampler, slot, samplerLocation);
}

void RenderContext::SetupUserFragment2DArrayTexture(const GX2Texture* texture, TextureSlot slot, FragmentTextureLocation samplerLocation) const
{
    _SetupFragment2DArrayTexture(texture, &textureSampler2.sampler, slot, samplerLocation);
}

void RenderContext::_SetupVertexTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, VertexTextureLocation samplerLocation) const
{
    //if (texture == NULL || samplerLocation.location == 0xFFFFFFFF)
    //    return;

    GX2SetVertexTexture(texture, samplerLocation.location);
    GX2SetVertexSampler(sampler, samplerLocation.location);
}

void RenderContext::SetupUserVertexTexture(const GX2Texture* texture, TextureSlot slot, VertexTextureLocation samplerLocation) const
{
    _SetupVertexTexture(texture, &textureSampler2.sampler, slot, samplerLocation);
}

void RenderContext::_SetupVertex2DArrayTexture(const GX2Texture* texture, const GX2Sampler* sampler, TextureSlot slot, VertexTextureLocation samplerLocation) const
{
    _SetupVertexTexture(texture, sampler, slot, samplerLocation);
}

void RenderContext::SetupVertexArrayTexture(const GX2Texture* texture, TextureSlot slot, VertexTextureLocation samplerLocation) const
{
    _SetupVertex2DArrayTexture(texture, &textureSampler2DArray.sampler, slot, samplerLocation);
}

} } // namespace nw::eft
