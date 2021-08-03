#ifndef EFT_SHADER_H_
#define EFT_SHADER_H_

#include <eft_CafeWrapper.h>
#include <eft_ResData.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

struct VertexShaderKey
{
    u8 _0[3];
    u8 shaderUserSetting;
    u32 flags[1];
    u8 _8[8];
    u32 shaderUserFlag;
    u32 shaderUserSwitchFlag;
    char shaderUserMacro[16];
};
static_assert(sizeof(VertexShaderKey) == 0x28, "VertexShaderKey size mismatch");

struct FragmentShaderKey
{
    u8 shaderMode;
    u8 textureMode;
    u8 colorMode;
    u8 alphaMode;
    u8 texture1ColorBlend;
    u8 texture1AlphaBlend;
    u8 texture2ColorBlend;
    u8 texture2AlphaBlend;
    u8 primitiveColorBlend;
    u8 primitiveAlphaBlend;
    u8 texture0ColorSrc;
    u8 texture1ColorSrc;
    u8 texture2ColorSrc;
    u8 primitiveColorSrc;
    u8 texture0AlphaSrc;
    u8 texture1AlphaSrc;
    u8 texture2AlphaSrc;
    u8 primitiveAlphaSrc;
    u8 shaderUserSetting;
    u8 texture0Dim;
    u8 texture1Dim;
    u8 texture2Dim;
    u32 shaderUserFlag;
    u32 shaderUserSwitchFlag;
    char shaderUserMacro[16];
    u8 _30[12];
};
static_assert(sizeof(FragmentShaderKey) == 0x3C, "FragmentShaderKey size mismatch");

struct GeometryShaderKey // Not certain
{
    u8 _unused[4];
};
static_assert(sizeof(GeometryShaderKey) == 4, "GeometryShaderKey size mismatch");

struct ShaderTable // Actual name not known
{
    u32 numShaderProg;
    u32 size;
    u32 shaderSrcOffs;
    u8 _unusedPad[4];
    u32 shaderBinOffs;
    u32 shaderProgOffs;
};
static_assert(sizeof(ShaderTable) == 0x18, "ShaderTable size mismatch");

struct ShaderProgram // Actual name not known
{
    VertexShaderKey vertexShaderKey;
    FragmentShaderKey fragmentShaderKey;
    GeometryShaderKey geometryShaderKey; // ?
    u32 binSize;
    u32 binOffs;
    u8 _unusedPad[8];
};
static_assert(sizeof(ShaderProgram) == 0x78, "ShaderProgram size mismatch");

struct VertexTextureLocation
{
    u32 location;
};
static_assert(sizeof(VertexTextureLocation) == 4, "VertexTextureLocation size mismatch");

struct FragmentTextureLocation
{
    u32 location;
};
static_assert(sizeof(FragmentTextureLocation) == 4, "FragmentTextureLocation size mismatch");

class Heap;
struct RenderStateSetArg;

class ParticleShader
{
public:
    enum UserUniformBlockID
    {
    };

public:
    ParticleShader();

    void Finalize(Heap* heap);
    void InitializeVertexShaderLocation();
    void InitializeFragmentShaderLocation();
    void InitializeAttribute();
    void InitializeStripeVertexShaderLocation();
    void InitializeStripeAttribute();
    bool SetupShaderResource(Heap* heap, void* binary, u32 binarySize);
    void Bind();
    bool SetUserVertexUniformBlock(UserUniformBlockID id, const char* name, void* buffer, s32 bufSize = -1, bool suppressWarning = false);
    bool SetUserFragmentUniformBlock(UserUniformBlockID id, const char* name, void* buffer, s32 bufSize = -1, bool suppressWarning = false);
    bool SetUserVertexUniformBlockFromData(RenderStateSetArg& arg, UserUniformBlockID id, const char* name);
    bool SetUserFragmentUniformBlockFromData(RenderStateSetArg& arg, UserUniformBlockID id, const char* name);
    bool SetUserUniformBlockFromData(RenderStateSetArg& arg, UserUniformBlockID id, const char* name);
    void EnableInstanced();
    void DisableInstanced();

    u8 displayList[512];
    u32 displatListSize;
    Shader shader;
    VertexShaderKey vertexShaderKey;
    FragmentShaderKey fragmentShaderKey;
    GeometryShaderKey geometryShaderKey; // ?
    u32 attrPosBuffer;
    u32 attrNormalBuffer;
    u32 attrColorBuffer;
    u32 attrTexCoordBuffer;
    u32 _unusedAttrBuffer0;
    u32 attrIndexBuffer;
    u32 attrRandomBuffer;
    u32 attrOuterBuffer;
    u32 attrDirBuffer;
    VertexTextureLocation vertexSamplerLocations[TextureSlot_Max];
    FragmentTextureLocation fragmentSamplerLocations[TextureSlot_Max];
    u32 attrSclBuffer;
    u32 attrVecBuffer;
    u32 attrWldPosBuffer;
    u32 attrWldPosDfBuffer;
    u32 attrColor0Buffer;
    u32 attrColor1Buffer;
    u32 attrRotBuffer;
    u32 attrEmMat0Buffer;
    u32 attrEmMat1Buffer;
    u32 attrEmMat2Buffer;
    u32 shaderAvailableAttribFlg;
    u32 attrUserAttr0Buffer;
    u32 attrUserAttr1Buffer;
    u32 attrUserAttr2Buffer;
    u32 attrUserAttr3Buffer;
    u32 attrStreamOutPosBuffer;
    u32 _unusedAttrBuffer1;
    u32 attrStreamOutVecBuffer;
    u32 _unusedAttrBuffer2;
    u8 _unusedPad[16];
    UniformBlock vertexViewUniformBlock;
    UniformBlock fragmentViewUniformBlock;
    UniformBlock vertexEmitterStaticUniformBlock;
    UniformBlock vertexEmitterDynamicUniformBlock;
    UniformBlock fragmentEmitterStaticUniformBlock;
    UniformBlock fragmentEmitterDynamicUniformBlock;
    UniformBlock stripeUniformBlock;
    UniformBlock vertexUserUniformBlocks[4];
    UniformBlock fragmentUserUniformBlocks[4];
    VertexTextureLocation vertexUserSamplerLocations[8];
    FragmentTextureLocation fragmentUserSamplerLocations[8];
};
static_assert(sizeof(ParticleShader) == 0x704, "ParticleShader size mismatch");

} } // namespace nw::eft

#endif // EFT_SHADER_H_
