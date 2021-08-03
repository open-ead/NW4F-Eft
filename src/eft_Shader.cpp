#include <eft_Emitter.h>
#include <eft_Misc.h>
#include <eft_Renderer.h>
#include <eft_Shader.h>
#include <eft_System.h>

namespace nw { namespace eft {

ParticleShader::ParticleShader()
{
    attrPosBuffer = 0xFFFFFFFF;
    attrNormalBuffer = 0xFFFFFFFF;
    attrColorBuffer = 0xFFFFFFFF;
    attrTexCoordBuffer = 0xFFFFFFFF;
    _unusedAttrBuffer0 = 0xFFFFFFFF;
    attrIndexBuffer = 0xFFFFFFFF;
    attrRandomBuffer = 0xFFFFFFFF;
    attrOuterBuffer = 0xFFFFFFFF;
    attrDirBuffer = 0xFFFFFFFF;
    attrSclBuffer = 0xFFFFFFFF;
    attrVecBuffer = 0xFFFFFFFF;
    attrWldPosBuffer = 0xFFFFFFFF;
    attrWldPosDfBuffer = 0xFFFFFFFF;
    attrColor0Buffer = 0xFFFFFFFF;
    attrColor1Buffer = 0xFFFFFFFF;
    attrRotBuffer = 0xFFFFFFFF;
    attrEmMat0Buffer = 0xFFFFFFFF;
    attrEmMat1Buffer = 0xFFFFFFFF;
    attrEmMat2Buffer = 0xFFFFFFFF;
    attrStreamOutPosBuffer = 0xFFFFFFFF;
    _unusedAttrBuffer1 = 0xFFFFFFFF;
    attrStreamOutVecBuffer = 0xFFFFFFFF;
    _unusedAttrBuffer2 = 0xFFFFFFFF;

    shaderAvailableAttribFlg = 0;

    for (u32 i = 0; i < TextureSlot_Max; i++)
    {
        vertexSamplerLocations[i].location = 0xFFFFFFFF;
        fragmentSamplerLocations[i].location = 0xFFFFFFFF;
    }

    for (u32 i = 0; i < 8u; i++)
    {
        vertexUserSamplerLocations[i].location = 0xFFFFFFFF;
        fragmentUserSamplerLocations[i].location = 0xFFFFFFFF;
    }

    displatListSize = 0;
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
    fragmentEmitterDynamicUniformBlock.InitializeVertexUniformBlock(&shader, "emitterDynamicUniformBlock", 5);
}

void ParticleShader::InitializeFragmentShaderLocation()
{
    fragmentViewUniformBlock.InitializePixelUniformBlock(&shader, "viewUniformBlock", 6);
    fragmentEmitterStaticUniformBlock.InitializePixelUniformBlock(&shader, "emitterStaticUniformBlock", 4);

    {
        if (fragmentShaderKey.texture0Dim == 1)
            fragmentSamplerLocations[0].location = shader.GetFragmentSamplerLocation("textureArraySampler0");
        else
            fragmentSamplerLocations[0].location = shader.GetFragmentSamplerLocation("textureSampler0");
    }

    if (fragmentShaderKey.textureMode == 1)
    {
        if (fragmentShaderKey.texture1Dim == 1)
            fragmentSamplerLocations[1].location = shader.GetFragmentSamplerLocation("textureArraySampler1");
        else
            fragmentSamplerLocations[1].location = shader.GetFragmentSamplerLocation("textureSampler1");
    }

    if (fragmentShaderKey.textureMode == 2)
    {
        if (fragmentShaderKey.texture1Dim == 1)
            fragmentSamplerLocations[1].location = shader.GetFragmentSamplerLocation("textureArraySampler1");
        else
            fragmentSamplerLocations[1].location = shader.GetFragmentSamplerLocation("textureSampler1");

      //if (fragmentShaderKey.texture2Dim == 1)
      //    fragmentSamplerLocations[2].location = shader.GetFragmentSamplerLocation("textureArraySampler2");
      //else
            fragmentSamplerLocations[2].location = shader.GetFragmentSamplerLocation("textureSampler2");
    }

    fragmentSamplerLocations[TextureSlot_Frame_Buffer].location = shader.GetFragmentSamplerLocation("frameBufferTexture");
    fragmentSamplerLocations[TextureSlot_Depth_Buffer].location = shader.GetFragmentSamplerLocation("depthBufferTexture");
    fragmentSamplerLocations[TextureSlot_Curl_Noise  ].location = shader.GetFragmentSamplerLocation("textureArrayCurlNoiseSampler");

    fragmentSamplerLocations[TextureSlot_User_Slot_0 ].location = shader.GetFragmentSamplerLocation("userTextureSampler0");
    fragmentSamplerLocations[TextureSlot_User_Slot_1 ].location = shader.GetFragmentSamplerLocation("userTextureSampler1");
    fragmentSamplerLocations[TextureSlot_User_Slot_2 ].location = shader.GetFragmentSamplerLocation("userTextureSampler2");
    fragmentSamplerLocations[TextureSlot_User_Slot_3 ].location = shader.GetFragmentSamplerLocation("userTextureSampler3");

    vertexSamplerLocations[TextureSlot_Frame_Buffer].location = shader.GetVertexSamplerLocation("frameBufferTexture");
    vertexSamplerLocations[TextureSlot_Depth_Buffer].location = shader.GetVertexSamplerLocation("depthBufferTexture");
    vertexSamplerLocations[TextureSlot_Curl_Noise  ].location = shader.GetVertexSamplerLocation("textureArrayCurlNoiseSampler");

    vertexSamplerLocations[TextureSlot_User_Slot_0 ].location = shader.GetVertexSamplerLocation("userTextureSampler0");
    vertexSamplerLocations[TextureSlot_User_Slot_1 ].location = shader.GetVertexSamplerLocation("userTextureSampler1");
    vertexSamplerLocations[TextureSlot_User_Slot_2 ].location = shader.GetVertexSamplerLocation("userTextureSampler2");
    vertexSamplerLocations[TextureSlot_User_Slot_3 ].location = shader.GetVertexSamplerLocation("userTextureSampler3");
}

void ParticleShader::InitializeAttribute()
{
    attrPosBuffer          = shader.GetAttribute("v_inPos",           0, Shader::VertexFormat_VEC4,  0, false);
    if (!(vertexShaderKey.flags[0] & 0x200000))
        attrIndexBuffer    = shader.GetAttribute("v_inIndex",         1, Shader::VertexFormat_U32,   0, false);
    attrTexCoordBuffer     = shader.GetAttribute("v_inTexCoord",      2, Shader::VertexFormat_VEC4,  0, false);
    attrNormalBuffer       = shader.GetAttribute("v_inNormal",        3, Shader::VertexFormat_VEC3,  0, false);
    attrColorBuffer        = shader.GetAttribute("v_inColor",         4, Shader::VertexFormat_VEC4,  0, false);
    attrWldPosBuffer       = shader.GetAttribute("v_inWldPos",        5, Shader::VertexFormat_VEC4,  0, true);
    attrSclBuffer          = shader.GetAttribute("v_inScl",           5, Shader::VertexFormat_VEC4,  4, true);
    attrVecBuffer          = shader.GetAttribute("v_inVec",           5, Shader::VertexFormat_VEC4,  8, true);
    attrRandomBuffer       = shader.GetAttribute("v_inRandom",        5, Shader::VertexFormat_VEC4, 12, true);
    attrRotBuffer          = shader.GetAttribute("v_inRot",           5, Shader::VertexFormat_VEC4, 16, true);
    attrEmMat0Buffer       = shader.GetAttribute("v_inEmtMat0",       5, Shader::VertexFormat_VEC4, 20, true);
    attrEmMat1Buffer       = shader.GetAttribute("v_inEmtMat1",       5, Shader::VertexFormat_VEC4, 24, true);
    attrEmMat2Buffer       = shader.GetAttribute("v_inEmtMat2",       5, Shader::VertexFormat_VEC4, 28, true);
    attrColor0Buffer       = shader.GetAttribute("v_inColor0",        5, Shader::VertexFormat_VEC4, 32, true);
    attrColor1Buffer       = shader.GetAttribute("v_inColor1",        5, Shader::VertexFormat_VEC4, 36, true);
    attrWldPosDfBuffer     = shader.GetAttribute("v_inWldPosDf",      5, Shader::VertexFormat_VEC4, 40, true);
    attrUserAttr0Buffer    = shader.GetAttribute("v_inUserAttr0",     5, Shader::VertexFormat_VEC4,  0, true);
    attrUserAttr1Buffer    = shader.GetAttribute("v_inUserAttr1",     6, Shader::VertexFormat_VEC4,  0, true);
    attrUserAttr2Buffer    = shader.GetAttribute("v_inUserAttr2",     7, Shader::VertexFormat_VEC4,  0, true);
    attrUserAttr3Buffer    = shader.GetAttribute("v_inUserAttr3",     8, Shader::VertexFormat_VEC4,  0, true);
    attrStreamOutPosBuffer = shader.GetAttribute("v_inStreamOutPos",  9, Shader::VertexFormat_VEC4,  0, true, true);
    attrStreamOutVecBuffer = shader.GetAttribute("v_inStreamOutVec", 10, Shader::VertexFormat_VEC4,  0, true, true);

    if (attrSclBuffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_Scl;

    if (attrWldPosBuffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_WldPos;

    if (attrWldPosDfBuffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_WldPosDf;

    if (attrColor0Buffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_Color0;

    if (attrColor1Buffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_Color1;

    if (attrRotBuffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_Rot;

    if (attrEmMat0Buffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_EmMat;

    if (attrRandomBuffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_Random;

    if (attrVecBuffer != 0xFFFFFFFF)
        shaderAvailableAttribFlg |= ShaderAttrib_Vec;
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

    if (!(vertexShaderKey.flags[0] & 0x80))
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

bool ParticleShader::SetUserVertexUniformBlock(UserUniformBlockID id, const char* name, void* buffer, s32 bufSize, bool suppressWarning)
{
    u32 userIdx = id - (UserUniformBlockID)9;

    if (!vertexUserUniformBlocks[userIdx].blockNotExist && !vertexUserUniformBlocks[userIdx].initialized
        && !vertexUserUniformBlocks[userIdx].InitializeVertexUniformBlock(&shader, name, id))
    {
        if (!suppressWarning)
            WARNING("User Vertex UniformBlock Setup Failed !! : %s. \n", name);

        return false;
    }

    if (vertexUserUniformBlocks[userIdx].initialized)
        vertexUserUniformBlocks[userIdx].BindUniformBlock(buffer, bufSize);

    return true;
}

bool ParticleShader::SetUserFragmentUniformBlock(UserUniformBlockID id, const char* name, void* buffer, s32 bufSize, bool suppressWarning)
{
    u32 userIdx = id - (UserUniformBlockID)9;

    if (!fragmentUserUniformBlocks[userIdx].blockNotExist && !fragmentUserUniformBlocks[userIdx].initialized
        && !fragmentUserUniformBlocks[userIdx].InitializePixelUniformBlock(&shader, name, id))
    {
        if (!suppressWarning)
            WARNING("User Fragment UniformBlock Setup Failed !! : %s. \n", name);

        return false;
    }

    if (fragmentUserUniformBlocks[userIdx].initialized)
        fragmentUserUniformBlocks[userIdx].BindUniformBlock(buffer, bufSize);

    return true;
}

bool ParticleShader::SetUserVertexUniformBlockFromData(RenderStateSetArg& arg, UserUniformBlockID id, const char* name)
{
    Renderer* renderer = arg.renderer;
    const SimpleEmitterData* data = arg.emitter->data;
    const EmitterShaderParam* shaderParam;

    if (renderer->currentParticleType == PtclType_Child)
    {
        const ComplexEmitterData* cdata = static_cast<const ComplexEmitterData*>(data);
        const ChildData* childData = reinterpret_cast<const ChildData*>(cdata + 1);
        shaderParam = &childData->shaderParam;
    }
    else
    {
        shaderParam = &data->shaderParam;
    }

    f32* shaderParamData = static_cast<f32*>(renderer->AllocFromDoubleBuffer(sizeof(f32) * shaderParam->count));
    memcpy(shaderParamData, shaderParam->ptr, sizeof(f32) * shaderParam->count);

    GX2EndianSwap(shaderParamData, sizeof(f32) * shaderParam->count);
    GX2Invalidate(GX2_INVALIDATE_CPU_UNIFORM_BLOCK, shaderParamData, sizeof(f32) * shaderParam->count);

    return SetUserVertexUniformBlock(id, name, shaderParamData, -1, false);
}

bool ParticleShader::SetUserFragmentUniformBlockFromData(RenderStateSetArg& arg, UserUniformBlockID id, const char* name)
{
    Renderer* renderer = arg.renderer;
    const SimpleEmitterData* data = arg.emitter->data;
    const EmitterShaderParam* shaderParam;

    if (renderer->currentParticleType == PtclType_Child)
    {
        const ComplexEmitterData* cdata = static_cast<const ComplexEmitterData*>(data);
        const ChildData* childData = reinterpret_cast<const ChildData*>(cdata + 1);
        shaderParam = &childData->shaderParam;
    }
    else
    {
        shaderParam = &data->shaderParam;
    }

    f32* shaderParamData = static_cast<f32*>(renderer->AllocFromDoubleBuffer(sizeof(f32) * shaderParam->count));
    memcpy(shaderParamData, shaderParam->ptr, sizeof(f32) * shaderParam->count);

    GX2EndianSwap(shaderParamData, sizeof(f32) * shaderParam->count);
    GX2Invalidate(GX2_INVALIDATE_CPU_UNIFORM_BLOCK, shaderParamData, sizeof(f32) * shaderParam->count);

    return SetUserFragmentUniformBlock(id, name, shaderParamData, -1, false);
}

bool ParticleShader::SetUserUniformBlockFromData(RenderStateSetArg& arg, UserUniformBlockID id, const char* name)
{
    return SetUserVertexUniformBlockFromData(arg, id, name)
        && SetUserFragmentUniformBlockFromData(arg, id, name);
}

void ParticleShader::EnableInstanced()
{
}

void ParticleShader::DisableInstanced()
{
}

} } // namespace nw::eft
