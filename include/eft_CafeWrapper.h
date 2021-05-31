#ifndef EFT_CAFE_WRAPPER_H_
#define EFT_CAFE_WRAPPER_H_

#define INCLUDE_CAFE
#include <eft_Types.h>

namespace nw { namespace eft {

class Shader
{
public:
    Shader();

    GX2VertexShader* vertexShader;
    GX2PixelShader* pixelShader;
    GX2GeometryShader* geometryShader;
    GX2FetchShader fetchShader;
    void* fetchShaderBufPtr;
    u32 numAttribute;
    GX2AttribStream attributes[16];
    u32 attributeBuffer[16];
    bool initialized;
};
static_assert(sizeof(Shader) == 0x278, "Shader size mismatch");

class TextureSampler
{
public:
    TextureSampler();
    ~TextureSampler();

    GX2Sampler sampler;
};
static_assert(sizeof(TextureSampler) == 0xC, "TextureSampler size mismatch");

class VertexBuffer
{
public:
    VertexBuffer();

    void* AllocateVertexBuffer(Heap* heap, u32 bufSize, u32 size);
    void Invalidate();

    u32 _0;
    u32 size;
    u32 bufferSize;
    void* buffer;
};
static_assert(sizeof(VertexBuffer) == 0x10, "VertexBuffer size mismatch");

} } // namespace nw::eft

#endif // EFT_CAFE_WRAPPER_H_
