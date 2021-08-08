#ifndef EFT_RENDERER_H_
#define EFT_RENDERER_H_

#include <math/math_MTX44.h>
#include <math/math_VEC2.h>
#include <math/math_VEC3.h>
#include <eft_RenderContext.h>
#include <eft_TemporaryBuffer.h>

namespace nw { namespace eft {

class Config;
struct EmitterInstance;
class Heap;
class ParticleShader;
class Primitive;
struct PtclAttributeBuffer;
struct PtclAttributeBufferGpu;
struct PtclInstance;
struct PtclStripe;
struct StripeData;
struct StripeVertexBuffer;
class System;
struct ViewUniformBlock;

class Renderer
{
public:
    struct PtclViewZ // Actual name not known
    {
        PtclInstance* ptcl;
        f32 z;
        u32 idx;
    };
    static_assert(sizeof(PtclViewZ) == 0xC, "PtclViewZ size mismatch");

    static s32 ComparePtclViewZ(const void* a, const void* b);

public:
    Renderer(System* system, const Config& config);
    ~Renderer();

    void BeginRender(const math::MTX44& proj, const math::MTX34& view, const math::VEC3& cameraWorldPos, f32 zNear, f32 zFar);
    bool SetupParticleShaderAndVertex(ParticleShader* shader, MeshType meshType, Primitive* primitive);
    void SetupTexture(ParticleShader* shader, const TextureRes* texture0, const TextureRes* texture1, const nw::eft::TextureRes* texture2);
    void DrawCpuEntry(ParticleShader* shader, u32 primitiveMode, u32 firstVertex, u32 numInstances, PtclAttributeBuffer* ptclAttributeBuffer, Primitive* primitive);
    void DrawGpuEntry(ParticleShader* shader, u32 primitiveMode, u32 firstVertex, u32 numInstances, PtclAttributeBufferGpu* ptclAttributeBufferGpu, Primitive* primitive);
    void RequestParticle(const EmitterInstance* emitter, ParticleShader* shader, bool isChild, void* argData, bool draw = true);
    bool EntryChildParticleSub(const EmitterInstance* emitter, void* argData, bool draw = true);
    bool EntryParticleSub(const EmitterInstance* emitter, void* argData, bool draw = true);
    void EntryParticle(const EmitterInstance* emitter, void* argData);
    void BeginStremOut();
    void CalcStremOutParticle(const EmitterInstance* emitter, bool bind);
    void EndStremOut();
    void EndRender();

    void SwapDoubleBuffer();
    void* AllocFromDoubleBuffer(u32 size);
    void FlushCache();

    u32 MakeStripeAttributeBlockCore(PtclStripe* stripe, StripeVertexBuffer* stripeVertexBuffer, s32 firstVertex, const StripeData* stripeData);
    void GetPositionOnCubic(math::VEC3* result, const math::VEC3& startPos, const math::VEC3& startVel, const math::VEC3& endPos, const math::VEC3& endVel, f32 time);
    u32 MakeStripeAttributeBlockCoreDivide(PtclStripe* stripe, StripeVertexBuffer* stripeVertexBuffer, s32 firstVertex, const StripeData* stripeData);
    bool MakeStripeAttributeBlock(const EmitterInstance* emitter, PtclInstance* ptcl);
    bool ConnectionStripeUvScaleCalc(f32& invTexRatio, f32& texRatioSub, const EmitterInstance* emitter, s32 numParticles, f32 invRatio, s32 connectionType);
    u32 MakeConnectionStripeAttributeBlockCore(EmitterInstance* emitter, s32 numParticles, PtclInstance* ptclLast, PtclInstance* ptclBeforeLast, s32 connectionType, StripeVertexBuffer* stripeVertexBuffer);
    u32 MakeConnectionStripeAttributeBlockCoreDivide(EmitterInstance* emitter, s32 numParticles, PtclInstance* ptclLast, PtclInstance* ptclBeforeLast, s32 connectionType, StripeVertexBuffer* stripeVertexBuffer);
    StripeVertexBuffer* MakeConnectionStripeAttributeBlock(EmitterInstance* emitter);
    bool SetupStripeDrawSetting(const EmitterInstance* emitter, void* argData);
    void EntryConnectionStripe(const EmitterInstance* emitter, void* argData);
    void EntryStripe(const EmitterInstance* emitter, void* argData);

    void BindParticleAttributeBlock(PtclAttributeBuffer* ptclAttributeBuffer, ParticleShader* shader, u32 firstVertex, u32 numInstances);
    void BindGpuParticleAttributeBlock(PtclAttributeBufferGpu* ptclAttributeBufferGpu, ParticleShader* shader, u32 firstVertex, u32 numInstances);

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
    const GX2Texture* textures[14];
    math::VEC2 depthBufferTextureOffset;
    math::VEC2 depthBufferTextureScale;
    math::VEC2 frameBufferTextureOffset;
    math::VEC2 frameBufferTextureScale;
    TemporaryBuffer doubleBuffer;
    u32 stripeNumCalcVertex;
    u32 stripeNumDrawVertex;
    PtclType currentParticleType;
    ShaderType shaderType;
    u32 renderVisibilityFlags;
    u32 numDrawEmitter;
    u32 numDrawParticle;
    u8 _unused;
};
static_assert(sizeof(Renderer) == 0x27C, "Renderer size mismatch");

} } // namespace nw::eft

#endif // EFT_RENDERER_H_
