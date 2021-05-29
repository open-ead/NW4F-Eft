#ifndef EFT_EMITTER_COMPLEX_H_
#define EFT_EMITTER_COMPLEX_H_

#include <eft_EmitterSimple.h>

namespace nw { namespace eft {

class EmitterComplexCalc : public EmitterSimpleCalc
{
public:
    EmitterComplexCalc(System* system)
        : EmitterSimpleCalc(system)
    {
    }

    virtual void CalcEmitter(EmitterInstance* emitter) { EmitterSimpleCalc::CalcEmitter(emitter); }
    virtual EmitterType GetEmitterType() const { return EmitterType_Complex; }
    virtual u32 CalcParticle(EmitterInstance* emitter, CpuCore core, bool, bool);
    virtual u32 CalcChildParticle(EmitterInstance* emitter, CpuCore core, bool, bool);
};

} } // namespace nw::eft

#endif // EFT_EMITTER_COMPLEX_H_
