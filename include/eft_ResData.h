#ifndef EFT_RES_DATA_H_
#define EFT_RES_DATA_H_

#include <eft_Enum.h>

#define INCLUDE_CAFE
#include <eft_Types.h>

namespace nw { namespace eft {

struct TextureRes
{
    u16 width;
    u16 height;
    GX2TileMode tileMode;
    u32 swizzle;
    u8 _C[8];
    u8 wrapMode;
    u8 filterMode;
    u32 numMips;
    u32 compSel;
    u32 mipOffset[13];
    f32 maxLOD;
    f32 biasLOD;
    TextureResFormat originalTexFormat;
    u32 originalTexDataOffs;
    u32 originalTexDataSize;
    TextureResFormat cafeTexFormat;
    u32 cafeTexDataSize;
    u32 cafeTexDataOffs;
    u32 initialized;
    GX2Texture gx2Texture;
};
static_assert(sizeof(TextureRes) == 0x114, "TextureRes size mismatch");

struct EmitterData // Actual name not known
{
    EmitterType type;
    u32 flags;
    u32 seed;
    u32 userData;
    u32 _10;
    f32 _14[8];
    CustomActionCallBackID callbackID;
    u32 nameOffs;
    const char* name;
    TextureRes textures[2];
    void* keyAnim;
    u32 keyAnimOffs;
    u32 keyAnimSize;
    u32 _274;
    u32 _278;
    u32 primitiveIdx;
};
static_assert(sizeof(EmitterData) == 0x280, "EmitterData size mismatch");

struct SimpleEmitterData : EmitterData
{
    u8 _280[0x29C - 0x280];
    s32 _29C;
    u8 _2A0[0x410 - 0x2A0];
    s32 ptclMaxLifespan;
    u8 _414[0x6F4 - 0x414];
};
static_assert(sizeof(SimpleEmitterData) == 0x6F4, "SimpleEmitterData size mismatch");

struct ComplexEmitterData : SimpleEmitterData
{
    u32 childFlags;
    u16 _6F8;
    u16 _6FA;
    u16 stripeFlags;
    u8 _6FE[2];
    u16 childDataOffs;
    u16 _702;
    u16 _704;
    u16 stripeDataOffs;
    u32 _708;
};
static_assert(sizeof(ComplexEmitterData) == 0x70C, "ComplexEmitterData size mismatch");

struct ChildData
{
    u8 _0[0x48];
    TextureRes texture;
    u8 _15C[0x2FC - 0x15C];
};
static_assert(sizeof(ChildData) == 0x2FC, "ChildData size mismatch");

struct EmitterReference // Actual name not known
{
    u32 dataOffs;
    EmitterData* data;
};
static_assert(sizeof(EmitterReference) == 8, "EmitterReference size mismatch");

struct EmitterSetData // Actual name not known
{
    u32 userData;
    u32 _4;
    u32 nameOffs;
    const char* name;
    u32 numEmitter;
    u32 emitterRefOffs;
    EmitterReference* emitterRef;
};
static_assert(sizeof(EmitterSetData) == 0x1C, "EmitterSetData size mismatch");

struct Header // Actual name not known
{
    char magic[4];
    u32 version;
    u32 numEmitterSet;
    u32 _C;
    u32 strTblOffs;
    u32 textureDataTblOffs;
    u32 textureDataTblSize;
    u32 shaderTblOffs;
    u32 shaderTblSize;
    u32 keyAnimTblOffs;
    u32 keyAnimTblSize;
    u32 primitiveTblOffs;
    u32 primitiveTblSize;
    u32 _34;
    u32 _38;
    u32 _3C;
};
static_assert(sizeof(Header) == 0x40, "Header size mismatch");

struct PrimitiveAttrib // Actual name not known
{
    u32 count;
    u32 size;
    u32 bufferOffs;
    u32 bufferSize;
};
static_assert(sizeof(PrimitiveAttrib) == 0x10, "PrimitiveAttrib size mismatch");

struct PrimitiveData // Actual name not known
{
    PrimitiveAttrib pos;
    PrimitiveAttrib normal;
    PrimitiveAttrib color;
    PrimitiveAttrib texCoord;
    PrimitiveAttrib index;
};
static_assert(sizeof(PrimitiveData) == 0x50, "PrimitiveData size mismatch");

struct PrimitiveTable // Actual name not known
{
    u32 numPrimitive;
    u32 size;
    u32 primitiveOffs;
};
static_assert(sizeof(PrimitiveTable) == 0xC, "PrimitiveTable size mismatch");

} } // namespace nw::eft

#endif // EFT_RES_DATA_H_
