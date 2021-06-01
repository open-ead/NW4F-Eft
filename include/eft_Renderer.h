#ifndef EFT_RENDERER_H_
#define EFT_RENDERER_H_

#include <math/math_MTX44.h>
#include <math/math_VEC2.h>
#include <math/math_VEC3.h>
#include <eft_RenderContext.h>
#include <eft_TemporaryBuffer.h>

namespace nw { namespace eft {

class Config;
class Heap;
class Primitive;
class System;
struct ViewUniformBlock;

class Renderer
{
public:
    Renderer(Heap* heap, System* system, const Config& config);
    ~Renderer();

    System* system;
    Heap* heap;
    RenderContext renderContext;
    u32 primitiveMode;
    math::MTX44 view;
    math::MTX44 viewProj;
    math::MTX44 billboard;
    math::VEC3 eyeVec;
    math::VEC3 eyePos;
    VertexBuffer vbPos;
    VertexBuffer vbIndex;
    Primitive* primitive;
    ViewUniformBlock* viewUniformBlock;
    union
    {
        const GX2Texture* textures[2];
        struct
        {
            const GX2Texture* depthBufferTexture;
            const GX2Texture* frameBufferTexture;
        };
    };
    math::VEC2 depthBufferTextureOffset;
    math::VEC2 depthBufferTextureScale;
    math::VEC2 frameBufferTextureOffset;
    math::VEC2 frameBufferTextureScale;
    TemporaryBuffer doubleBuffer;
    u32 stripeNumDrawVertex;
    u32 currentParticleType; // 0 = Simple, 1 = Complex, 2 = Child, 3 = Invalid
    ShaderType shaderType;
    u32 _178;
};
static_assert(sizeof(Renderer) == 0x17C, "Renderer size mismatch");

} } // namespace nw::eft

#endif // EFT_RENDERER_H_
