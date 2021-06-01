#include <eft_Emitter.h>
#include <eft_EmitterSet.h>

namespace nw { namespace eft {

void EmitterController::SetFollowType(PtclFollowType followType)
{
    emitter->ptclFollowType = followType;
}

} } // namespace nw::eft
