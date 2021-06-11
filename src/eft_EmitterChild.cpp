#include <eft_EmitterComplex.h>
#include <eft_Particle.h>
#include <eft_System.h>

namespace nw { namespace eft {

void EmitterComplexCalc::EmitChildParticle(EmitterInstance* emitter, PtclInstance* ptcl)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);
    const ChildData* childData = reinterpret_cast<const ChildData*>(data + 1);

    for (s32 i = 0; i < childData->numChildParticles; i++)
    {
        PtclInstance* childPtcl = mSys->AllocPtcl(PtclType_Child);
        if (childPtcl == NULL)
            continue;

        childPtcl->data = data;
        childPtcl->stripe = NULL;
        childPtcl->lifespan = childData->ptclMaxLifespan;
        childPtcl->emitter = emitter;

        if (data->childFlags & 0x20)
            childPtcl->velocity = ptcl->posDiff * childData->velocityScaleFactor;

        else
            childPtcl->velocity = math::VEC3::Zero();

        if (data->childFlags & 4)
        {
            // ...
        }
    }
}

} } // namespace nw::eft
