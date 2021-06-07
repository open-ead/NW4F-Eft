#include <eft_Enum.h>
#include <eft_Shader.h>

namespace nw { namespace eft {

ParticleShader::ParticleShader()
{
    attrPosBuffer = 0xFFFFFFFF;
    attrNormalBuffer = 0xFFFFFFFF;
    attrColorBuffer = 0xFFFFFFFF;
    attrTexCoordBuffer = 0xFFFFFFFF;
    _4E0 = 0xFFFFFFFF;
    attrIndexBuffer = 0xFFFFFFFF;
    attrOuterBuffer = 0xFFFFFFFF;
    attrDirBuffer = 0xFFFFFFFF;
    attrSclBuffer = 0xFFFFFFFF;
    attrTexAnimBuffer = 0xFFFFFFFF;
    //attrSubTexAnimBuffer = 0xFFFFFFFF; <-- Nintendo forgot to do this
    attrWldPosBuffer = 0xFFFFFFFF;
    attrWldPosDfBuffer = 0xFFFFFFFF;
    attrColor0Buffer = 0xFFFFFFFF;
    attrColor1Buffer = 0xFFFFFFFF;
    attrRotBuffer = 0xFFFFFFFF;
    attrEmMat0Buffer = 0xFFFFFFFF;
    attrEmMat1Buffer = 0xFFFFFFFF;
    attrEmMat2Buffer = 0xFFFFFFFF;

    for (u32 i = 0; i < 2u; i++)
        fragmentSamplerLocations[i].location = 0xFFFFFFFF;

    fragmentDepthBufferSamplerLocation.location = 0xFFFFFFFF;
    fragmentFrameBufferSamplerLocation.location = 0xFFFFFFFF;

    for (u32 i = 0; i < 8u; i++)
    {
        vertexUserSamplerLocations[i].location = 0xFFFFFFFF;
        fragmentUserSamplerLocations[i].location = 0xFFFFFFFF;
    }
}

void ParticleShader::Finalize(Heap* heap)
{
    return shader.Finalize(heap);
}

void ParticleShader::InitializeVertexShaderLocation()
{
    vertexViewUniformBlock.InitializeVertexUniformBlock(&shader, "viewUniformBlock", 0);
    vertexEmitterStaticUniformBlock.InitializeVertexUniformBlock(&shader, "emitterStaticUniformBlock", 1);
    vertexEmitterDynamicUniformBlock.InitializeVertexUniformBlock(&shader, "emitterDynamicUniformBlock", 2);
}

void ParticleShader::InitializeFragmentShaderLocation()
{
    fragmentViewUniformBlock.InitializePixelUniformBlock(&shader, "viewUniformBlock", 5);
    fragmentEmitterStaticUniformBlock.InitializePixelUniformBlock(&shader, "emitterStaticUniformBlock", 4);

    fragmentSamplerLocations[0].location = shader.GetFragmentSamplerLocation("s_firstTexture");
    if (fragmentShaderKey.textureMode == 1)
        fragmentSamplerLocations[1].location = shader.GetFragmentSamplerLocation("s_secondTexture");

    fragmentDepthBufferSamplerLocation.location = shader.GetFragmentSamplerLocation("s_depthBufferTexture");
    fragmentFrameBufferSamplerLocation.location = shader.GetFragmentSamplerLocation("s_frameBufferTexture");
}

void ParticleShader::InitializeAttribute()
{
    attrPosBuffer        = shader.GetAttribute("v_inPos",        0, Shader::VertexFormat_VEC4,  0, false);
    if (!vertexShaderKey.isPrimitive)
        attrIndexBuffer  = shader.GetAttribute("v_inIndex",      1, Shader::VertexFormat_U32,   0, false);
    attrTexCoordBuffer   = shader.GetAttribute("v_inTexCoord",   2, Shader::VertexFormat_VEC4,  0, false);
    attrNormalBuffer     = shader.GetAttribute("v_inNormal",     3, Shader::VertexFormat_VEC3,  0, false);
    attrColorBuffer      = shader.GetAttribute("v_inColor",      4, Shader::VertexFormat_VEC4,  0, false);
    attrWldPosBuffer     = shader.GetAttribute("v_inWldPos",     5, Shader::VertexFormat_VEC4,  0, true);
    attrSclBuffer        = shader.GetAttribute("v_inScl",        5, Shader::VertexFormat_VEC4,  4, true);
    attrColor0Buffer     = shader.GetAttribute("v_inColor0",     5, Shader::VertexFormat_VEC4,  8, true);
    attrColor1Buffer     = shader.GetAttribute("v_inColor1",     5, Shader::VertexFormat_VEC4, 12, true);
    attrTexAnimBuffer    = shader.GetAttribute("v_inTexAnim",    5, Shader::VertexFormat_VEC4, 16, true);
    attrWldPosDfBuffer   = shader.GetAttribute("v_inWldPosDf",   5, Shader::VertexFormat_VEC4, 20, true);
    attrRotBuffer        = shader.GetAttribute("v_inRot",        5, Shader::VertexFormat_VEC4, 24, true);
    attrSubTexAnimBuffer = shader.GetAttribute("v_inSubTexAnim", 5, Shader::VertexFormat_VEC4, 28, true);
    attrEmMat0Buffer     = shader.GetAttribute("v_inEmtMat0",    5, Shader::VertexFormat_VEC4, 32, true);
    attrEmMat1Buffer     = shader.GetAttribute("v_inEmtMat1",    5, Shader::VertexFormat_VEC4, 36, true);
    attrEmMat2Buffer     = shader.GetAttribute("v_inEmtMat2",    5, Shader::VertexFormat_VEC4, 40, true);
}

void ParticleShader::InitializeStripeVertexShaderLocation()
{
    vertexViewUniformBlock.InitializeVertexUniformBlock(&shader, "viewUniformBlock", 0);
    stripeUniformBlock.InitializeVertexUniformBlock(&shader, "stripeUniformBlock", 3);
}

void ParticleShader::InitializeStripeAttribute()
{
    attrPosBuffer      = shader.GetAttribute("v_inPos",      0, Shader::VertexFormat_VEC4,  0, false);
    attrOuterBuffer    = shader.GetAttribute("v_inOuter",    0, Shader::VertexFormat_VEC4,  4, false);
    attrTexCoordBuffer = shader.GetAttribute("v_inTexCoord", 0, Shader::VertexFormat_VEC4,  8, false);
    attrDirBuffer      = shader.GetAttribute("v_inDir",      0, Shader::VertexFormat_VEC4, 12, false);
}

bool ParticleShader::SetupShaderResource(Heap* heap, void* binary, u32 binarySize)
{
    shader.CreateShader(heap, binary, binarySize);

    if (vertexShaderKey.transformMode != VertexTransformMode_Stripe
        && vertexShaderKey.transformMode != VertexTransformMode_Complex_Stripe)
    {
        InitializeVertexShaderLocation();
        InitializeFragmentShaderLocation();
        InitializeAttribute();
    }
    else
    {
        InitializeStripeVertexShaderLocation();
        InitializeFragmentShaderLocation();
        InitializeStripeAttribute();
    }

    shader.SetupShader(heap);

    memset(displayList, 0, 512);
    DCFlushRange(displayList, 512);
    DCInvalidateRange(displayList, 512);
    GX2BeginDisplayList(displayList, 512);

    shader.BindShader();

    displatListSize = GX2EndDisplayList(displayList);

    return true;
}

void ParticleShader::Bind()
{
    GX2CallDisplayList(displayList, displatListSize);
}

void ParticleShader::EnableInstanced()
{
}

void ParticleShader::DisableInstanced()
{
}

} } // namespace nw::eft
