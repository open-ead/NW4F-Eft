#include <eft_Config.h>
#include <eft_Heap.h>
#include <eft_Renderer.h>

namespace nw { namespace eft {

Renderer::Renderer(Heap* argHeap, System* argSystem, const Config& config)
    : system(argSystem)
{
    depthBufferTextureOffset.x = 0.0f;
    depthBufferTextureOffset.y = 0.0f;
    depthBufferTextureScale.x = 1.0f;
    depthBufferTextureScale.y = 1.0f;
    frameBufferTextureOffset.x = 0.0f;
    frameBufferTextureOffset.y = 0.0f;
    frameBufferTextureScale.x = 1.0f;
    frameBufferTextureScale.y = 1.0f;

    heap = argHeap;

    depthBufferTexture = NULL;
    frameBufferTexture = NULL;

    currentParticleType = 3;
    shaderType = ShaderType_Normal;
    _178 = 0x3F;

    math::VEC3* pos = static_cast<nw::math::VEC3*>(vbPos.AllocateVertexBuffer(argHeap, sizeof(nw::math::VEC3) * 4, 3));
    pos[0].x = -0.5f;
    pos[0].y = 0.5f;
    pos[0].z = 0.0f;
    pos[1].x = -0.5f;
    pos[1].y = -0.5f;
    pos[1].z = 0.0f;
    pos[2].x = 0.5f;
    pos[2].y = -0.5f;
    pos[2].z = 0.0f;
    pos[3].x = 0.5f;
    pos[3].y = 0.5f;
    pos[3].z = 0.0f;
    vbPos.Invalidate();

    u32* index = static_cast<u32*>(vbIndex.AllocateVertexBuffer(argHeap, sizeof(u32) * 4, 1));
    index[0] = 0;
    index[1] = 1;
    index[2] = 2;
    index[3] = 3;
    vbIndex.Invalidate();

    primitive = NULL;

    u32 doubleBufferSize = config.doubleBufferSize;
    doubleBufferSize += config.numParticleMax *  0xCC;
    doubleBufferSize += config.numEmitterMax  * 0x114;

    doubleBuffer.Initialize(heap, doubleBufferSize);
}

} } // namespace nw::eft
