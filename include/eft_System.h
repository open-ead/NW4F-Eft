#ifndef EFT_SYSTEM_H_
#define EFT_SYSTEM_H_

#include <eft_Enum.h>

#include <types.h>

namespace nw { namespace eft {

class Config;
class EmitterCalc;
class EmitterInstance;
class EmitterSet;
class EmitterStaticUniformBlock;
class Heap;
class PtclInstance;
class PtclStripe;
class Renderer;
class Resource;

// TODO: move this somewhere else
struct Matrix44f
{
    f32 rows[4][4];
};

class System
{
public:
    System(const Config& config);
    virtual ~System(); // deleted

    virtual void Initialize(Heap* heap, const Config& config);

    bool initialized;
    Heap* heap;
    Resource** resources;
    u32 numResourceMax;
    EmitterSet* emitterSets;
    u32 numEmitterSetMax;
    u8 _18[4];
    Matrix44f view[CpuCore_Max];
    EmitterInstance* emitterGroups[64];  // Maximum of 64 groups
    EmitterSet* emitterSetGroupHead[64]; // Maximum of 64 groups
    EmitterSet* emitterSetGroupTail[64]; // Maximum of 64 groups
    EmitterInstance* emitters;
    EmitterStaticUniformBlock* emitterStaticUniformBlocks;
    u8 _3E4[4];
    s32 numUnusedEmitters;
    PtclInstance* particles;
    s32 currentPtclIdx;
    u8 _3F4[8];
    PtclInstance** childParticles[CpuCore_Max];
    u32 numChildParticles[CpuCore_Max];
    PtclInstance** ptclToRemove[CpuCore_Max];
    u32 numPtclToRemove[CpuCore_Max];
    PtclStripe* stripes;
    PtclStripe* stripeGroups[64]; // Maximum of 64 groups
    u8 _530[0xC];
    u32 numEmitterMax;
    u32 numParticleMax;
    u32 numStripeMax;
    u8 _548[4];
    u32 _54C;
    u8 _550[8];
    u32 numCalcPtcl;
    u32 numCalcEmitterSet;
    u32 numEmittedPtcl;
    u8 _564[4];
    u64 activeGroupsFlg;
    u32 _570[CpuCore_Max][64]; // Maximum of 64 groups
    Renderer* renderers[CpuCore_Max];
    u32 numCreatedEmitterSet;
    EmitterCalc* emitterCalc[2];
    void* resourceWork;
    void* emitterSetWork;
    void* rendererWork[CpuCore_Max];
    void* emitterSimpleCalcWork;
    void* emitterComplexCalcWork;
    u8 _8A4[4];
    s32 _8A8;
    CustomActionCallBackID currentCallbackID;
    void* customActionEmitterPreCalcCallback[CustomActionCallBackID_Max];
    void* customActionParticleEmitCallback[CustomActionCallBackID_Max];
    void* customActionParticleRemoveCallback[CustomActionCallBackID_Max];
    void* customActionParticleCalcCallback[CustomActionCallBackID_Max];
    void* customActionParticleMakeAttrCallback[CustomActionCallBackID_Max];
    void* customActionEmitterPostCalcCallback[CustomActionCallBackID_Max];
    void* customActionEmitterDrawOverrideCallback[CustomActionCallBackID_Max];
    void* customShaderEmitterCalcPostCallback[CustomShaderCallBackID_Max];
    void* customShaderDrawOverrideCallback[CustomShaderCallBackID_Max];
    void* customShaderRenderStateSetCallback[CustomShaderCallBackID_Max];
    void* sortedEmitterSets[CpuCore_Max];
    u32 numSortedEmitterSets[CpuCore_Max];
    u32 _A14[CpuCore_Max];
    u8 _A20[4];
};

} } // namespace nw::eft

#endif // EFT_SYSTEM_H_
