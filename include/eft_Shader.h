#ifndef EFT_SHADER_H_
#define EFT_SHADER_H_

#include <eft_CafeWrapper.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

struct VertexShaderKey
{
    u8 transformMode;
    u8 rotationMode;
    u8 shaderUserSetting;
    u8 stripeType;
    bool stripeEmitterCoord;
    bool isPrimitive;
    u32 shaderUserFlag;
    u32 shaderUserSwitchFlag;
    char shaderUserMacro[16];
};
static_assert(sizeof(VertexShaderKey) == 0x20, "VertexShaderKey size mismatch");

struct FragmentShaderKey
{
    u8 shaderMode;
    bool softEdge;
    u8 textureMode;
    u8 colorMode;
    u8 alphaMode;
    u8 shaderUserSetting;
    bool isPrimitive;
    u8 textureColorBlend;
    u8 textureAlphaBlend;
    u8 primitiveColorBlend;
    u8 primitiveAlphaBlend;
    u8 texture0ColorSrc;
    u8 texture1ColorSrc;
    u8 primitiveColorSrc;
    u8 texture0AlphaSrc;
    u8 texture1AlphaSrc;
    u8 primitiveAlphaSrc;
    u8 reflectionApplyAlpha;
    u16 _12; // Unused
    u32 shaderUserFlag;
    u32 shaderUserSwitchFlag;
    char shaderUserMacro[16];
    u16 _2C; // Unused
    //u8 pad[2];
};
static_assert(sizeof(FragmentShaderKey) == 0x30, "FragmentShaderKey size mismatch");

struct GeometryShaderKey // Not certain
{
    u8 _0[4];
};
static_assert(sizeof(GeometryShaderKey) == 4, "GeometryShaderKey size mismatch");

struct ShaderTable // Actual name not known
{
    u32 numShaderProg;
    u32 size;
    u32 shaderSrcOffs;
    u32 shaderProgOffs;
};
static_assert(sizeof(ShaderTable) == 0x10, "ShaderTable size mismatch");

struct ShaderProgram // Actual name not known
{
    VertexShaderKey vertexShaderKey;
    FragmentShaderKey fragmentShaderKey;
    GeometryShaderKey geometryShaderKey; // ?
    u32 binSize;
    u32 binOffs;
};
static_assert(sizeof(ShaderProgram) == 0x5C, "ShaderProgram size mismatch");

class Heap;

class ParticleShader
{
public:
    ParticleShader();

    bool SetupShaderResource(Heap* heap, void* binary, u32 binarySize);

    u8 displayList[512];
    u32 displatListSize;
    Shader shader;
    VertexShaderKey vertexShaderKey;
    FragmentShaderKey fragmentShaderKey;
    GeometryShaderKey geometryShaderKey; // ?
    s32 attrPosLocation;
    s32 attrNormalLocation;
    s32 attrColorLocation;
    s32 attrTexCoordLocation;
    s32 _4E0; // Unused
    s32 attrIndexLocation;
    s32 attrOuterLocation;
    s32 attrDirLocation;
    s32 fragmentSamplerLocations[4];
    s32 attrSclLocation;
    s32 attrTexAnimLocation;
    s32 attrSubTexAnimLocation;
    s32 attrWldPosLocation;
    s32 attrWldPosDfLocation;
    s32 attrColor0Location;
    s32 attrColor1Location;
    s32 attrRotLocation;
    s32 attrEmMat0Location;
    s32 attrEmMat1Location;
    s32 attrEmMat2Location;
    UniformBlock vertexViewUniformBlock;
    UniformBlock fragmentViewUniformBlock;
    UniformBlock vertexEmitterStaticUniformBlock;
    UniformBlock vertexEmitterDynamicUniformBlock;
    UniformBlock _56C; // Unused?
    UniformBlock fragmentEmitterStaticUniformBlock;
    UniformBlock stripeUniformBlock;
    UniformBlock vertexUserUniformBlocks[2];
    UniformBlock fragmentUserUniformBlock[2];
    s32 vertexUserSamplerLocations[8];
    s32 fragmentUserSamplerLocations[8];
};

} }

#endif // EFT_SHADER_H_
