#include <math/math_Triangular.h>
#include <eft_Emitter.h>
#include <eft_Heap.h>

namespace nw { namespace eft {

void EmitterCalc::InitializeFluctuationTable(Heap* heap)
{
    sFluctuationTbl = static_cast<f32*>(heap->Alloc(sizeof(f32) * 128));

    for (s32 i = 0; i < 128; i++)
        sFluctuationTbl[i] = nw::math::cosRad(i / 128.0f * 2.0f * 3.14159f) * 0.5f + 0.5f;
}

} } // namespace nw::eft
