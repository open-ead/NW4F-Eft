#ifndef EFT_SYSTEM_H_
#define EFT_SYSTEM_H_

#include <math/math_MTX34.h>
#include <math/math_MTX44.h>
#include <eft_Enum.h>

namespace nw { namespace eft {

struct AlphaAnim;
class Config;
class EmitterCalc;
struct EmitterInstance;
class EmitterSet;
struct EmitterStaticUniformBlock;
class Handle;
class Heap;
struct PtclInstance;
class PtclStripe;
class Renderer;
class Resource;
struct ScaleAnim; // ?
struct SimpleEmitterData;

class System
{
public:
    struct PtclViewZ // Actual name not known
    {
        EmitterSet* emitterSet;
        u32 z;
    };

public:
    System(const Config& config);
    virtual ~System(); // deleted

    virtual void Initialize(Heap* heap, const Config& config);

    EmitterSet* AllocEmitterSet(Handle* handle);
    EmitterInstance* AllocEmitter(u8 groupID);
    void AddEmitterSetToDrawList(EmitterSet* emitterSet, u8 groupID);
    void InitializeEmitter(EmitterInstance* emitter, const SimpleEmitterData* data, u32 resourceID, s32 emitterSetID, u32 seed, bool keepCreateID);
    bool CreateEmitterSetID(Handle* handle, const math::MTX34& matrixSRT, s32 emitterSetID, u32 resourceID, u8 groupID, u32 emitterEnableMask = 0xFFFFFFFF);
    void ClearResource(Heap* heap, u32 resourceID);
    void EntryResource(Heap* heap, void* resource, u32 resourceID);

    // For qsort
    static int ComparePtclViewZ(const void* a, const void* b);

    bool initialized;
    Heap* heap;
    Resource** resources;
    u32 numResourceMax;
    EmitterSet* emitterSets;
    s32 numEmitterSetMax;
    s32 numEmitterSetMaxMask;
    math::MTX44 view[CpuCore_Max];
    EmitterInstance* emitterGroups[64];  // Maximum of 64 groups
    EmitterSet* emitterSetGroupHead[64]; // Maximum of 64 groups
    EmitterSet* emitterSetGroupTail[64]; // Maximum of 64 groups
    EmitterInstance* emitters;
    EmitterStaticUniformBlock* emitterStaticUniformBlocks;
    s32 currentEmitterIdx;
    s32 numUnusedEmitters;
    PtclInstance* particles;
    s32 currentParticleIdx;
    AlphaAnim* alphaAnim;
    ScaleAnim* scaleAnim; // ?
    PtclInstance** childParticles[CpuCore_Max];
    u32 numChildParticle[CpuCore_Max];
    PtclInstance** particlesToRemove[CpuCore_Max];
    u32 numParticleToRemove[CpuCore_Max];
    PtclStripe* stripes;
    PtclStripe* stripeGroups[64]; // Maximum of 64 groups
    u32 _530;
    s32 currentEmitterSetIdx;
    u32 _538;
    s32 numEmitterMax;
    s32 numParticleMax;
    s32 numStripeMax;
    s32 numEmitterMaxMask;
    s32 numParticleMaxMask;
    s32 numStripeMaxMask;
    u32 numCalcEmitter;
    u32 numCalcParticle;
    u32 numCalcEmitterSet;
    u32 numEmittedParticle;
    u32 _564;
    u64 activeGroupsFlg;
    u32 _570[CpuCore_Max][64]; // Maximum of 64 groups
    Renderer* renderers[CpuCore_Max];
    u32 currentEmitterSetCreateID;
    EmitterCalc* emitterCalc[EmitterType_Max];
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
    PtclViewZ* sortedEmitterSets[CpuCore_Max];
    u32 numSortedEmitterSets[CpuCore_Max];
    u32 _A14[CpuCore_Max];
    u32 _A20;
};

} } // namespace nw::eft

#endif // EFT_SYSTEM_H_
