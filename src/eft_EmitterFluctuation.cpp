#include <math/math_Triangular.h>
#include <eft_Emitter.h>
#include <eft_Misc.h>

namespace nw { namespace eft {

f32* EmitterCalc::sFluctuationTbl = NULL;
f32* EmitterCalc::sFluctuationSawToothTbl = NULL;
f32* EmitterCalc::sFluctuationRectTbl = NULL;

void EmitterCalc::InitializeFluctuationTable()
{
    sFluctuationTbl = static_cast<f32*>(AllocFromStaticHeap(sizeof(f32) * 128));
    sFluctuationSawToothTbl = static_cast<f32*>(AllocFromStaticHeap(sizeof(f32) * 128));
    sFluctuationRectTbl = static_cast<f32*>(AllocFromStaticHeap(sizeof(f32) * 128));

    for (s32 i = 0; i < 128; i++)
        sFluctuationTbl[i] = nw::math::CosRad(i / 128.0f * 2.0f * 3.14159f) * 0.5f + 0.5f;

    for (s32 i = 0; i < 128; i++)
        sFluctuationSawToothTbl[i] = i / 128.0f;

    for (s32 i = 0; i < 128; i++)
        if (i < 64)
            sFluctuationRectTbl[i] = 1.0f;
        else
            sFluctuationRectTbl[i] = 0.0f;
}

void EmitterCalc::CalcFluctuation(EmitterInstance* emitter, PtclInstance* ptcl)
{
    const ComplexEmitterData* data = static_cast<const ComplexEmitterData*>(emitter->data);
    const FluctuationData* fluctuationData = reinterpret_cast<const FluctuationData*>((u32)data + data->fluctuationDataOffs);

    f32 fluxX = 1.0f;
    s32 idxX = (s32)((ptcl->counterS32 + fluctuationData->x.phase + 128.0f / fluctuationData->x.frequency * ptcl->randomVec4.x * fluctuationData->x.enableRandom) * fluctuationData->x.frequency) & 127;

    if (data->fluctuationFlags & 8)
        fluxX = 1.0f - sFluctuationTbl[idxX] * fluctuationData->x.amplitude;

    if (data->fluctuationFlags & 0x10)
        fluxX = 1.0f - sFluctuationSawToothTbl[idxX] * fluctuationData->x.amplitude;

    if (data->fluctuationFlags & 0x20)
        fluxX = 1.0f - sFluctuationRectTbl[idxX] * fluctuationData->x.amplitude;

    if (data->fluctuationFlags & 2) ptcl->fluctuationAlpha = fluxX;
    if (data->fluctuationFlags & 4) ptcl->fluctuationScale.x = fluxX;

    if (data->fluctuationFlags & 0x40)
    {
        f32 fluxY = 1.0f;
        s32 idxY = (s32)((ptcl->counterS32 + fluctuationData->y.phase + 128.0f / fluctuationData->y.frequency * ptcl->randomVec4.x * fluctuationData->y.enableRandom) * fluctuationData->y.frequency) & 127;

        if (data->fluctuationFlags & 8)
            fluxY = 1.0f - sFluctuationTbl[idxY] * fluctuationData->y.amplitude;

        if (data->fluctuationFlags & 0x10)
            fluxY = 1.0f - sFluctuationSawToothTbl[idxY] * fluctuationData->y.amplitude;

        if (data->fluctuationFlags & 0x20)
            fluxY = 1.0f - sFluctuationRectTbl[idxY] * fluctuationData->y.amplitude;

        if (data->fluctuationFlags & 4) ptcl->fluctuationScale.y = fluxY;
    }
    else
    {
        if (data->fluctuationFlags & 4) ptcl->fluctuationScale.y = fluxX;
    }
}

} } // namespace nw::eft
