#ifndef EFT_ENUM_H_
#define EFT_ENUM_H_

namespace nw { namespace eft {

enum BlendType
{
    BlendType_Type0 = 0,
    BlendType_Type1 = 1,
    BlendType_Type2 = 2,
    BlendType_Type3 = 3,
    BlendType_Type4 = 4,
    BlendType_Max = 5,
};

enum CpuCore
{
    CpuCore_0 = 0,
    CpuCore_1 = 1,
    CpuCore_2 = 2,
    CpuCore_Max = 3,
};

enum CustomActionCallBackID
{
    CustomActionCallBackID_Invalid = 0xFFFFFFFF,
    CustomActionCallBackID_Max = 8,
};

enum CustomShaderCallBackID
{
    CustomShaderCallBackID_Max = 9,
};

enum EmitterType // Actual name not known
{
    EmitterType_Simple = 0,
    EmitterType_Complex = 1,
    EmitterType_Max = 2,
};

enum PtclFollowType
{
    PtclFollowType_SRT = 0,
    PtclFollowType_None = 1,
    PtclFollowType_Trans = 2,
    PtclFollowType_Max = 3,
};

enum ShaderType
{
    ShaderType_Normal = 0,
    ShaderType_UserMacro1 = 1,
    ShaderType_UserMacro2 = 2,
    ShaderType_Max,
};

enum TextureResFormat // Actual name not known
{
    TextureResFormat_Invalid = 0,
    TextureResFormat_RGB8_Unorm = 1,
    TextureResFormat_RGBA8_Unorm = 2,
    TextureResFormat_BC1_Unorm = 3,
    TextureResFormat_BC1_SRGB = 4,
    TextureResFormat_BC2_Unorm = 5,
    TextureResFormat_BC2_SRGB = 6,
    TextureResFormat_BC3_Unorm = 7,
    TextureResFormat_BC3_SRGB = 8,
    TextureResFormat_BC4_Unorm = 9,
    TextureResFormat_BC4_Snorm = 10,
    TextureResFormat_BC5_Unorm = 11,
    TextureResFormat_BC5_Snorm = 12,
    TextureResFormat_R8_Unorm = 13,
    TextureResFormat_RG8_Unorm = 14,
    TextureResFormat_RGBA8_SRGB = 15,
    TextureResFormat_Max,
};

enum ZBufATestType
{
    ZBufATestType_Type0 = 0,
    ZBufATestType_Type1 = 1,
    ZBufATestType_Type2 = 2,
    ZBufATestType_Max = 3,
};

} } // namespace nw::eft

#endif // EFT_ENUM_H_
