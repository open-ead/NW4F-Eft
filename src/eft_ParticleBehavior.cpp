#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_UniformBlock.h>

namespace nw { namespace eft {

void EmitterCalc::MakeParticleAttributeBuffer(PtclAttributeBuffer* ptclAttributeBuffer, PtclInstance* ptcl, u32 shaderAvailableAttribFlg, f32 cameraOffset)
{
    const EmitterSet* emitterSet = ptcl->emitter->emitterSet;

    ptclAttributeBuffer->wldPos.xyz() = ptcl->worldPos;
    ptclAttributeBuffer->wldPos.w = cameraOffset;

    ptclAttributeBuffer->scl.x = ptcl->scale.x * emitterSet->_220.x * ptcl->_B0;
    ptclAttributeBuffer->scl.y = ptcl->scale.y * emitterSet->_220.y * ptcl->_B0;
    ptclAttributeBuffer->scl.z = ptcl->texAnimParam[0].rotate;
    ptclAttributeBuffer->scl.w = ptcl->texAnimParam[1].rotate;

    ptclAttributeBuffer->color0.xyz() = ptcl->color0.rgb();
    ptclAttributeBuffer->color0.w = ptcl->alpha * ptcl->_AC;

    if (shaderAvailableAttribFlg & 0x40)
    {
        ptclAttributeBuffer->color1.xyz() = ptcl->color1.rgb();
        ptclAttributeBuffer->color1.w = ptcl->alpha * ptcl->_AC;
    }

    ptclAttributeBuffer->texAnim.x    = ptcl->texAnimParam[0].offset.x + ptcl->texAnimParam[0].scroll.x;
    ptclAttributeBuffer->texAnim.y    = ptcl->texAnimParam[0].offset.y - ptcl->texAnimParam[0].scroll.y;
    ptclAttributeBuffer->texAnim.zw() = ptcl->texAnimParam[0].scale;

    if (shaderAvailableAttribFlg & 0x10)
        ptclAttributeBuffer->wldPosDf.xyz() = ptcl->worldPosDiff;

    if (shaderAvailableAttribFlg & 0x80)
    {
        ptclAttributeBuffer->rot.xyz() = ptcl->rotation;
        ptclAttributeBuffer->rot.w = 0.0f;
    }

    if (shaderAvailableAttribFlg & 4)
    {
        ptclAttributeBuffer->subTexAnim.x    = ptcl->texAnimParam[1].offset.x + ptcl->texAnimParam[1].scroll.x;
        ptclAttributeBuffer->subTexAnim.y    = ptcl->texAnimParam[1].offset.y - ptcl->texAnimParam[1].scroll.y;
        ptclAttributeBuffer->subTexAnim.zw() = ptcl->texAnimParam[1].scale;
    }

    if (shaderAvailableAttribFlg & 0x100)
        ptclAttributeBuffer->emtMat = *ptcl->pMatrixRT;
}

} } // namespace nw::eft
