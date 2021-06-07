#include <eft_Renderer.h>
#include <eft_Shader.h>

namespace nw { namespace eft {

void Renderer::BindParticleAttributeBlock(PtclAttributeBuffer* ptclAttributeBuffer, ParticleShader* shader, u32 numInstances)
{
    VertexBuffer::BindExtBuffer(5, sizeof(PtclAttributeBuffer) * numInstances, 0, sizeof(PtclAttributeBuffer), ptclAttributeBuffer);
}

} } // namespace nw::eft
