#ifndef EFT_CAFE_WRAPPER_H_
#define EFT_CAFE_WRAPPER_H_

#include <cafe.h>

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

class TextureSampler
{
public:
    TextureSampler();
    ~TextureSampler();

    GX2Sampler sampler;
};

} } // namespace nw::eft

#endif // EFT_CAFE_WRAPPER_H_
