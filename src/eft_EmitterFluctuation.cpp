#include <math/math_Triangular.h>
#include <eft_Emitter.h>
#include <eft_Heap.h>

namespace nw { namespace eft {

f32* EmitterCalc::sFluctuationTbl = NULL;

void EmitterCalc::InitializeFluctuationTable(Heap* heap)
{
    sFluctuationTbl = static_cast<f32*>(heap->Alloc(sizeof(f32) * 128));

    for (s32 i = 0; i < 128; i++)
        sFluctuationTbl[i] = nw::math::CosRad(i / 128.0f * 2.0f * 3.14159f) * 0.5f + 0.5f;
}

void EmitterCalc::CalcFluctuation(EmitterInstance* emitter, PtclInstance* ptcl)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);
    const FluctuationData* fluctuationData = reinterpret_cast<const FluctuationData*>((u32)data + data->fluctuationDataOffs);

    s32 idx = ((s32)((s32)ptcl->counter * fluctuationData->frequency) + ptcl->randomU32 * fluctuationData->enableRandom) & 127;
    f32 flux = 1.0f - sFluctuationTbl[idx] * fluctuationData->amplitude;

    if (data->fluctuationFlags & 2) ptcl->fluctuationAlpha = flux;
    if (data->fluctuationFlags & 4) ptcl->fluctuationScale = flux;
}

} } // namespace nw::eft
