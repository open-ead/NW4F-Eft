#ifndef EFT_SYSTEM_H_
#define EFT_SYSTEM_H_

#include <math/math_MTX34.h>
#include <math/math_MTX44.h>
#include <eft_Enum.h>

namespace nw { namespace eft {

struct EmitterInstance;
struct PtclInstance;

struct EmitterPreCalcArg
{
    EmitterInstance* emitter;
};

struct EmitterPostCalcArg
{
    EmitterInstance* emitter;
};

struct ParticleRemoveArg
{
    PtclInstance* ptcl;
};

struct ShaderEmitterPostCalcArg
{
    EmitterInstance* emitter;
    bool noCalcBehavior;
    bool childParticle;
};

typedef void (*CustomActionEmitterPreCalcCallback)(EmitterPreCalcArg& arg);
typedef void (*CustomActionEmitterPostCalcCallback)(EmitterPostCalcArg& arg);
typedef bool (*CustomActionParticleRemoveCallback)(ParticleRemoveArg& arg);
typedef void (*CustomShaderEmitterPostCalcCallback)(ShaderEmitterPostCalcArg& arg);

struct AlphaAnim;
class Config;
class EmitterCalc;
class EmitterSet;
struct EmitterStaticUniformBlock;
class Handle;
class Heap;
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
    static_assert(sizeof(PtclViewZ) == 8, "PtclViewZ size mismatch");

public:
    System(const Config& config);
    virtual ~System(); // deleted

    virtual void Initialize(Heap* heap, const Config& config);

    void RemoveStripe(PtclStripe* stripe);
    void RemovePtcl_();
    EmitterSet* RemoveEmitterSetFromDrawList(EmitterSet* emitterSet);
    void RemovePtcl();
    void EmitChildParticle();
    EmitterSet* AllocEmitterSet(Handle* handle);
    EmitterInstance* AllocEmitter(u8 groupID);
    void AddEmitterSetToDrawList(EmitterSet* emitterSet, u8 groupID);

    void InitializeEmitter(EmitterInstance* emitter, const SimpleEmitterData* data, u32 resourceID, s32 emitterSetID, u32 seed, bool keepCreateID);
    bool CreateEmitterSetID(Handle* handle, const math::MTX34& matrixSRT, s32 emitterSetID, u32 resourceID, u8 groupID, u32 emitterEnableMask = 0xFFFFFFFF);

    void BeginFrame();
    void SwapDoubleBuffer();
    void CalcEmitter(u8 groupID, f32 emissionSpeed = 1.0f);
    void CalcParticle(EmitterInstance* emitter, CpuCore core);
    void CalcChildParticle(EmitterInstance* emitter, CpuCore core);
    void FlushCache();
    void FlushGpuCache();
    inline void CalcEmitter(EmitterInstance* emitter, f32 emissionSpeed);
    void CalcParticle(bool flushCache);
    void Calc(bool flushCache);

    void ClearResource(Heap* heap, u32 resourceID);
    void EntryResource(Heap* heap, void* resource, u32 resourceID);

    CustomActionEmitterPreCalcCallback GetCurrentCustomActionEmitterPreCalcCallback(const EmitterInstance* emitter);
    CustomActionEmitterPostCalcCallback GetCurrentCustomActionEmitterPostCalcCallback(const EmitterInstance* emitter);
    CustomActionParticleRemoveCallback GetCurrentCustomActionParticleRemoveCallback(const EmitterInstance* emitter);
    CustomShaderEmitterPostCalcCallback GetCustomShaderEmitterPostCalcCallback(CustomShaderCallBackID callbackID);

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
    s32 numChildParticle[CpuCore_Max];
    PtclInstance** particlesToRemove[CpuCore_Max];
    s32 numParticleToRemove[CpuCore_Max];
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
    u32 numCalcStripe;
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
    CustomActionEmitterPreCalcCallback customActionEmitterPreCalcCallback[CustomActionCallBackID_Max];
    void* customActionParticleEmitCallback[CustomActionCallBackID_Max];
    CustomActionParticleRemoveCallback customActionParticleRemoveCallback[CustomActionCallBackID_Max];
    void* customActionParticleCalcCallback[CustomActionCallBackID_Max];
    void* customActionParticleMakeAttrCallback[CustomActionCallBackID_Max];
    CustomActionEmitterPostCalcCallback customActionEmitterPostCalcCallback[CustomActionCallBackID_Max];
    void* customActionEmitterDrawOverrideCallback[CustomActionCallBackID_Max];
    CustomShaderEmitterPostCalcCallback customShaderEmitterPostCalcCallback[CustomShaderCallBackID_Max];
    void* customShaderDrawOverrideCallback[CustomShaderCallBackID_Max];
    void* customShaderRenderStateSetCallback[CustomShaderCallBackID_Max];
    PtclViewZ* sortedEmitterSets[CpuCore_Max];
    u32 numSortedEmitterSets[CpuCore_Max];
    u32 _A14[CpuCore_Max];
    u32 doubleBufferSwapped;
};
static_assert(sizeof(System) == 0xA28, "System size mismatch");

} } // namespace nw::eft

#endif // EFT_SYSTEM_H_
