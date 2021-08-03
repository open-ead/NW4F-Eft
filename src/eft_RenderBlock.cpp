#include <eft_Renderer.h>
#include <eft_Shader.h>

namespace nw { namespace eft {

void Renderer::BindParticleAttributeBlock(PtclAttributeBuffer* ptclAttributeBuffer, ParticleShader* shader, u32 firstVertex, u32 numInstances)
{
    VertexBuffer::BindExtBuffer(5, sizeof(PtclAttributeBuffer) * numInstances, 0, sizeof(PtclAttributeBuffer), &ptclAttributeBuffer[firstVertex]);
}

void Renderer::BindGpuParticleAttributeBlock(PtclAttributeBufferGpu* ptclAttributeBufferGpu, ParticleShader* shader, u32 firstVertex, u32 numInstances)
{
    VertexBuffer::BindExtBuffer(5, sizeof(PtclAttributeBufferGpu) * numInstances, 0, sizeof(PtclAttributeBufferGpu), &ptclAttributeBufferGpu[firstVertex]);
}

} } // namespace nw::eft
