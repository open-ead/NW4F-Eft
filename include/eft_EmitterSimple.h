#ifndef EFT_EMITTER_SIMPLE_H_
#define EFT_EMITTER_SIMPLE_H_

#include <eft_Emitter.h>

namespace nw { namespace eft {

class EmitterSimpleCalc : public EmitterCalc
{
public:
    EmitterSimpleCalc(System* system)
        : EmitterCalc(system)
    {
    }

    virtual void CalcEmitter(EmitterInstance* emitter);
    virtual EmitterType GetEmitterType() const { return EmitterType_Simple; }
    virtual u32 CalcParticle(EmitterInstance* emitter, CpuCore core, bool, bool);
    virtual u32 CalcChildParticle(EmitterInstance* emitter, CpuCore core, bool, bool);
};
static_assert(sizeof(EmitterSimpleCalc) == 4, "EmitterSimpleCalc size mismatch");

} } // namespace nw::eft

#endif // EFT_EMITTER_SIMPLE_H_
