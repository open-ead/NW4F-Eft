#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_System.h>

namespace nw { namespace eft {

void EmitterController::SetFollowType(PtclFollowType followType)
{
    emitter->ptclFollowType = followType;
}

void EmitterSet::Kill()
{
    system->KillEmitterSet(this);
}

} } // namespace nw::eft
