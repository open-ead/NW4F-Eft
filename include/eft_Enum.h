#ifndef EFT_ENUM_H_
#define EFT_ENUM_H_

#include <eft_Types.h>

namespace nw { namespace eft {

enum AlphaSourceType // Actual name not known
{
    AlphaSourceType_First  = 0,
    AlphaSourceType_3v4k   = 1,
    AlphaSourceType_8key   = 2,
    AlphaSourceType_Max    = 3,
};
static_assert(sizeof(AlphaSourceType) == 4, "AlphaSourceType size mismatch");

enum AnimationType // Actual name not known
{
    AnimationType_None = 0,
    AnimationType_3v4k = 1,
    AnimationType_8key = 2,
    AnimationType_Max  = 3,
};
static_assert(sizeof(AnimationType) == 4, "AnimationType size mismatch");

enum BlendType
{
    BlendType_Normal = 0,
    BlendType_Add    = 1,
    BlendType_Sub    = 2,
    BlendType_Screen = 3,
    BlendType_Mult   = 4,
    BlendType_User   = 5,
    BlendType_Max    = 6,
};
static_assert(sizeof(BlendType) == 4, "BlendType size mismatch");

enum ColorSourceType // Actual name not known
{
    ColorSourceType_First  = 0,
    ColorSourceType_Random = 1,
    ColorSourceType_3v4k   = 2,
    ColorSourceType_8key   = 3,
    ColorSourceType_Max    = 4,
};
static_assert(sizeof(ColorSourceType) == 4, "ColorSourceType size mismatch");

enum CpuCore
{
    CpuCore_0   = 0,
    CpuCore_1   = 1,
    CpuCore_2   = 2,
    CpuCore_Max = 3,
};
static_assert(sizeof(CpuCore) == 4, "CpuCore size mismatch");

enum CustomActionCallBackID
{
    CustomActionCallBackID_Invalid = 0,
    CustomActionCallBackID_Max     = 9,
};
static_assert(sizeof(CustomActionCallBackID) == 4, "CustomActionCallBackID size mismatch");

enum CustomShaderCallBackID
{
    CustomShaderCallBackID_Invalid = 0,
    CustomShaderCallBackID_Max     = 9,
};
static_assert(sizeof(CustomShaderCallBackID) == 4, "CustomShaderCallBackID size mismatch");

enum DisplaySideType
{
    DisplaySideType_Both  = 0,
    DisplaySideType_Front = 1,
    DisplaySideType_Back  = 2,
    DisplaySideType_Max   = 3,
};
static_assert(sizeof(DisplaySideType) == 4, "DisplaySideType size mismatch");

enum DrawPathCallback
{
    DrawPathCallback_Max = 8,
};
static_assert(sizeof(DrawPathCallback) == 4, "DrawPathCallback size mismatch");

enum DrawPathFlag
{
    DrawPathFlag_Force_8bytes = 0x7FFFFFFFFFFFFFFF,
};
static_assert(sizeof(DrawPathFlag) == 8, "DrawPathFlag size mismatch");

enum EmitterBehaviorFlag
{
    EmitterBehaviorFlag_HasTransAnim  = 0x01,
    EmitterBehaviorFlag_HasRotateAnim = 0x02,
    EmitterBehaviorFlag_HasSRTAnim    = 0x04,
    EmitterBehaviorFlag_IsEmitted     = 0x08,
    EmitterBehaviorFlag_IsCalculated  = 0x10,
    EmitterBehaviorFlag_PrevPosSet    = 0x20,
};
static_assert(sizeof(EmitterBehaviorFlag) == 4, "EmitterBehaviorFlag size mismatch");

enum EmitterType // Actual name not known
{
    EmitterType_Simple    = 0,
    EmitterType_Complex   = 1,
    EmitterType_SimpleGpu = 2,
    EmitterType_Max       = 3,
};
static_assert(sizeof(EmitterType) == 4, "EmitterType size mismatch");

enum FragmentComposite // Actual name not known
{
    FragmentComposite_Mul = 0,
    FragmentComposite_Add = 1,
    FragmentComposite_Sub = 2,
    FragmentComposite_Max = 3,
};
static_assert(sizeof(FragmentComposite) == 4, "FragmentComposite size mismatch");

enum MeshType
{
    MeshType_Particle  = 0,
    MeshType_Primitive = 1,
    MeshType_Stripe    = 2,
    MeshType_Max       = 3,
};
static_assert(sizeof(MeshType) == 4, "MeshType size mismatch");

enum ParticleBehaviorFlag
{
    ParticleBehaviorFlag_AirResist       = 0x0001,
    ParticleBehaviorFlag_Gravity         = 0x0002,
    ParticleBehaviorFlag_Rotate          = 0x0004,
    ParticleBehaviorFlag_RotInertia      = 0x0008,
    ParticleBehaviorFlag_WldPosDf        = 0x0010,
    ParticleBehaviorFlag_ScaleAnim       = 0x0040,
    //ParticleBehaviorFlag_AlphaAnim       = 0x0080,
    //ParticleBehaviorFlag_Color0Anim      = 0x0100,
    //ParticleBehaviorFlag_Color1Anim      = 0x0200,
    //ParticleBehaviorFlag_Tex0UVShiftAnim = 0x0400,
    //ParticleBehaviorFlag_Tex1UVShiftAnim = 0x0800,
    //ParticleBehaviorFlag_Tex0PtnAnim     = 0x1000,
    //ParticleBehaviorFlag_Tex1PtnAnim     = 0x2000,
    //ParticleBehaviorFlag_HasTex1         = 0x4000,
    ParticleBehaviorFlag_Unk             = 0x80000,
};
static_assert(sizeof(ParticleBehaviorFlag) == 4, "ParticleBehaviorFlag size mismatch");

enum PtclFollowType
{
    PtclFollowType_SRT   = 0,
    PtclFollowType_None  = 1,
    PtclFollowType_Trans = 2,
    PtclFollowType_Max   = 3,
};
static_assert(sizeof(PtclFollowType) == 4, "PtclFollowType size mismatch");

enum PtclType // Actual name unknown
{
    PtclType_Simple  = 0,
    PtclType_Complex = 1,
    PtclType_Child   = 2,
    PtclType_Max     = 4,
};
static_assert(sizeof(PtclFollowType) == 4, "PtclFollowType size mismatch");

enum ShaderAttrib
{
    ShaderAttrib_Scl        = 0x001,
    ShaderAttrib_WldPos     = 0x002,
    ShaderAttrib_WldPosDf   = 0x004,
    ShaderAttrib_Color0     = 0x008,
    ShaderAttrib_Color1     = 0x010,
    ShaderAttrib_Rot        = 0x020,
    ShaderAttrib_EmMat      = 0x040,
    ShaderAttrib_Random     = 0x080,
    ShaderAttrib_Vec        = 0x100,
};
static_assert(sizeof(ShaderAttrib) == 4, "ShaderAttrib size mismatch");

enum ShaderType
{
    ShaderType_Normal     = 0,
    ShaderType_UserMacro1 = 1,
    ShaderType_UserMacro2 = 2,
    ShaderType_Max        = 3,
};
static_assert(sizeof(ShaderType) == 4, "ShaderType size mismatch");

enum TextureFilterMode
{
    TextureFilterMode_Linear = 0,
    TextureFilterMode_Point  = 1,
    TextureFilterMode_Max    = 2,
};
static_assert(sizeof(TextureFilterMode) == 4, "TextureFilterMode size mismatch");

enum TextureResFormat // Actual name not known
{
    TextureResFormat_Invalid       = 0,
    TextureResFormat_RGB8_Unorm    = 1,
    TextureResFormat_RGBA8_Unorm   = 2,
    TextureResFormat_BC1_Unorm     = 3,
    TextureResFormat_BC1_SRGB      = 4,
    TextureResFormat_BC2_Unorm     = 5,
    TextureResFormat_BC2_SRGB      = 6,
    TextureResFormat_BC3_Unorm     = 7,
    TextureResFormat_BC3_SRGB      = 8,
    TextureResFormat_BC4_Unorm     = 9,
    TextureResFormat_BC4_Snorm     = 10,
    TextureResFormat_BC5_Unorm     = 11,
    TextureResFormat_BC5_Snorm     = 12,
    TextureResFormat_R8_Unorm      = 13,
    TextureResFormat_RG8_Unorm     = 14,
    TextureResFormat_RGBA8_SRGB    = 15,
    TextureResFormat_R8_Snorm      = 16,
    TextureResFormat_RG4_Unorm     = 17,
    TextureResFormat_RG11B10_Float = 18,
    TextureResFormat_R16_Float     = 19,
    TextureResFormat_RG16_Float    = 20,
    TextureResFormat_RGBA16_Float  = 21,
    TextureResFormat_R32_Float     = 22,
    TextureResFormat_RG32_Float    = 23,
    TextureResFormat_RGBA32_Float  = 24,
    TextureResFormat_RGB565_Unorm  = 25,
    TextureResFormat_RGB5A1_Unorm  = 26,
    TextureResFormat_Max           = 27,
};
static_assert(sizeof(TextureResFormat) == 4, "TextureResFormat size mismatch");

enum TextureSlot
{
    TextureSlot_0            = 0,
    TextureSlot_1            = 1,
    TextureSlot_2            = 2,
    TextureSlot_Frame_Buffer = 3,
    TextureSlot_Depth_Buffer = 4,
    TextureSlot_Curl_Noise   = 5,
    // 6
    TextureSlot_User_Slot_0  = 7,
    TextureSlot_User_Slot_1  = 8,
    TextureSlot_User_Slot_2  = 9,
    TextureSlot_User_Slot_3  = 10,
    // 11
    // 12
    // 13
    TextureSlot_Max          = 14,
};
static_assert(sizeof(TextureSlot) == 4, "TextureSlot size mismatch");

enum TextureWrapMode
{
    TextureWrapMode_Mirror      = 0,
    TextureWrapMode_Wrap        = 1,
    TextureWrapMode_Clamp       = 2,
    TextureWrapMode_Mirror_Once = 3,
    TextureWrapMode_Max         = 4,
};
static_assert(sizeof(TextureWrapMode) == 4, "TextureWrapMode size mismatch");

enum VertexRotationMode // Actual name not known
{
    VertexRotationMode_None       = 0,
    VertexRotationMode_Rotate_X   = 1,
    VertexRotationMode_Rotate_Y   = 2,
    VertexRotationMode_Rotate_Z   = 3,
    VertexRotationMode_Rotate_YZX = 4,
    VertexRotationMode_Rotate_XYZ = 5,
    VertexRotationMode_Rotate_ZXY = 6,
    VertexRotationMode_Max        = 7,
};
static_assert(sizeof(VertexRotationMode) == 4, "VertexRotationMode size mismatch");

enum VertexTransformMode // Actual name not known
{
    VertexTransformMode_Billboard           = 0,
    VertexTransformMode_Plate_XY            = 1,
    VertexTransformMode_Plate_XZ            = 2,
    VertexTransformMode_Directional_Y       = 3,
    VertexTransformMode_Directional_Polygon = 4,
    VertexTransformMode_Stripe              = 5,
    VertexTransformMode_Complex_Stripe      = 6,
    VertexTransformMode_Primitive           = 7,
    VertexTransformMode_Y_Billboard         = 8,
    VertexTransformMode_Complex_Billboard   = 9,
};
static_assert(sizeof(VertexTransformMode) == 4, "VertexTransformMode size mismatch");

enum ZBufATestType
{
    ZBufATestType_Normal   = 0,
    ZBufATestType_Ignore_Z = 1,
    ZBufATestType_Alpha    = 2,
    ZBufATestType_Opaque   = 3,
    ZBufATestType_Max      = 4,
};
static_assert(sizeof(ZBufATestType) == 4, "ZBufATestType size mismatch");

} } // namespace nw::eft

#endif // EFT_ENUM_H_
