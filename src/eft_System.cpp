#include <eft_Config.h>
#include <eft_EmitterComplex.h>
#include <eft_EmitterFieldCurlNoise.h>
#include <eft_EmitterSet.h>
#include <eft_EmitterSimpleGpu.h>
#include <eft_Handle.h>
#include <eft_Heap.h>
#include <eft_Misc.h>
#include <eft_Particle.h>
#include <eft_Random.h>
#include <eft_Renderer.h>
#include <eft_System.h>
#include <eft_UniformBlock.h>

#include <cstring>
#include <new>

#include <nn/util/detail/util_Symbol.h>

namespace nw { namespace eft {

bool System::mInitialized = false;

System::System(const Config& config)
{
    NN_UTIL_REFER_SYMBOL("[SDK+NINTENDO:NW4F_1_11_0_eft]");

    if (config.GetEffectHeap())
        Initialize(config.GetEffectHeap(), config.GetEffectHeap(), config);
}

System::~System()
{
}

void System::Initialize(Heap* heap, Heap* dynamicHeap, const Config& config)
{
    numResourceMax = config.numResourceMax + config.numExtraResourceMax;
    numEmitterMax = config.numEmitterMax;
    numParticleMax = config.numParticleMax;
    numEmitterSetMax = config.numEmitterSetMax;
    numStripeMax = config.numStripeMax;

    SetStaticHeap(heap);
    SetDynamicHeap(dynamicHeap);
    InitializeDelayFreeList(numEmitterMax * 3);
    SetSuppressOutputLog(config.suppressOutputLog);

    numCalcStripe = 0;
    numUnusedEmitters = numEmitterMax;
    numCalcEmitter = 0;
    currentEmitterIdx = 0;
    currentParticleIdx = 0;
    numCalcParticle = 0;
    counter = 0;
    currentEmitterSetCreateID = 0;
    currentStripeIdx = 0;
    numCalcParticleGpu = 0;
    currentEmitterSetIdx = 0;
    currentCallbackID = CustomActionCallBackID_Invalid;
    _unused0 = 0;
    numCalcEmitterSet = 0;
    doubleBufferSwapped = 0;

    memset(emitterGroups,           0, 64 * sizeof(EmitterInstance*));
    memset(emitterSetGroupHead,     0, 64 * sizeof(EmitterSet*));
    memset(emitterSetGroupTail,     0, 64 * sizeof(EmitterSet*));
    memset(stripeGroups,            0, 64 * sizeof(PtclStripe*));
    memset(_unusedFlags[CpuCore_0], 0, 64 * sizeof(u32));
    memset(_unusedFlags[CpuCore_1], 0, 64 * sizeof(u32));
    memset(_unusedFlags[CpuCore_2], 0, 64 * sizeof(u32));

    for (u32 i = 0; i < 64u; i++)
        streamOutParam[i] = 1;

    PtclRandom::Initialize();

    InitializeCurlNoise();

    resourceWork = heap->Alloc(sizeof(Resource*) * numResourceMax);
    resources = new (resourceWork) Resource*[numResourceMax];
    memset(resources, 0, sizeof(Resource*) * numResourceMax);

    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        rendererWork[i] = heap->Alloc(sizeof(Renderer));
        renderers[i] = new (rendererWork[i]) Renderer(this, config);
        renderers[i]->textures[TextureSlot_Curl_Noise] = GetCurlNoiseTexture();
    }

    emitterSetWork = heap->Alloc(sizeof(EmitterSet) * numEmitterSetMax + 32);
    emitterSets = new (emitterSetWork) EmitterSet[numEmitterSetMax];

    for (s32 i = 0; i < numEmitterSetMax; i++)
        emitterSets[i].system = this;

    emitters = static_cast<EmitterInstance*>(heap->Alloc(sizeof(EmitterInstance) * numEmitterMax));

    for (s32 i = 0; i < numEmitterMax; i++)
    {
        emitters[i].calc = NULL;
        emitters[i].emitterStaticUniformBlock = NULL;
        emitters[i].childEmitterStaticUniformBlock = NULL;
        emitters[i].ptclAttributeBuffer = NULL;
        emitters[i].ptclAttributeBufferGpu = NULL;
        emitters[i].numPtclAttributeBufferGpuMax = 0;
        emitters[i].posStreamOutAttributeBuffer.Invalidate();
        emitters[i].vecStreamOutAttributeBuffer.Invalidate();
        emitters[i].swapStreamOut = true;
    }

    particles = static_cast<PtclInstance*>(heap->Alloc(sizeof(PtclInstance) * numParticleMax));
    alphaAnim[0] = static_cast<AlphaAnim*>(heap->Alloc(sizeof(AlphaAnim) * numParticleMax));
    alphaAnim[1] = static_cast<AlphaAnim*>(heap->Alloc(sizeof(AlphaAnim) * numParticleMax));
    scaleAnim = static_cast<ScaleAnim*>(heap->Alloc(sizeof(ScaleAnim) * numParticleMax));
    complexParam = static_cast<ComplexEmitterParam*>(heap->Alloc(sizeof(ComplexEmitterParam) * numParticleMax));

    for (s32 i = 0; i < numParticleMax; i++)
    {
        particles[i].data = NULL;
        particles[i].alphaAnim[0] = &alphaAnim[0][i];
        particles[i].alphaAnim[1] = &alphaAnim[1][i];
        particles[i].scaleAnim = &scaleAnim[i];
        particles[i].complexParam = &complexParam[i];
    }

    emitterSimpleCalcWork = heap->Alloc(sizeof(EmitterSimpleCalc));
    emitterCalc[EmitterType_Simple] = new (emitterSimpleCalcWork) EmitterSimpleCalc(this);

    emitterComplexCalcWork = heap->Alloc(sizeof(EmitterComplexCalc));
    emitterCalc[EmitterType_Complex] = new (emitterComplexCalcWork) EmitterComplexCalc(this);

    emitterSimpleGpuCalcWork = heap->Alloc(sizeof(EmitterSimpleGpuCalc));
    emitterCalc[EmitterType_SimpleGpu] = new (emitterSimpleGpuCalcWork) EmitterSimpleGpuCalc(this);

    EmitterCalc::InitializeFluctuationTable();

    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        childParticles[i] = static_cast<PtclInstance**>(heap->Alloc(sizeof(PtclInstance*) * numParticleMax));
        numChildParticle[i] = 0;

        for (s32 j = 0; j < numParticleMax; j++)
            childParticles[i][j] = NULL;
    }

    stripes = static_cast<PtclStripe*>(heap->Alloc(sizeof(PtclStripe) * numStripeMax));

    for (s32 i = 0; i < numStripeMax; i++)
    {
        stripes[i].data = NULL;
        stripes[i].queueFront = 0;
        stripes[i].queueRear = 0;
        stripes[i].numDraw = 0;
    }

    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        numStripeToRemove[i] = 0;
        stripesToRemove[i] = static_cast<PtclStripe**>(heap->Alloc(sizeof(PtclStripe*) * numStripeMax));

        for (u32 j = 0; j < numStripeMax; j++)
            stripesToRemove[i][j] = NULL;
    }

    for (u32 i = 0; i < CustomActionCallBackID_Max; i++)
    {
        customActionEmitterMatrixSetCallback[i] = NULL;
        customActionEmitterPreCalcCallback[i] = NULL;
        customActionParticleEmitCallback[i] = NULL;
        customActionParticleRemoveCallback[i] = NULL;
        customActionParticleCalcCallback[i] = NULL;
        customActionParticleMakeAttributeCallback[i] = NULL;
        customActionEmitterPostCalcCallback[i] = NULL;
        customActionEmitterDrawOverrideCallback[i] = NULL;
    }

    for (u32 i = 0; i < CustomShaderCallBackID_Max; i++)
    {
        customShaderEmitterInitializeCallback[i] = NULL;
        customShaderEmitterFinalizeCallback[i] = NULL;
        customShaderEmitterPreCalcCallback[i] = NULL;
        customShaderEmitterPostCalcCallback[i] = NULL;
        customShaderDrawOverrideCallback[i] = NULL;
        customShaderRenderStateSetCallback[i] = NULL;
    }

    for (u32 i = 0; i < DrawPathCallback_Max; i++)
    {
        drawPathCallbackFlags[i] = 0;
        drawPathRenderStateSetCallback[i] = NULL;
    }

    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        sortedEmitterSets[i] = static_cast<PtclViewZ*>(heap->Alloc(sizeof(PtclViewZ) * numEmitterSetMax));
        numSortedEmitterSets[i] = 0;
        _unused1[i] = 0;
    }

    isSharedPlaneEnable = 0;
    sharedPlaneX = (math::VEC2){ -50, 50 };
    sharedPlaneY = 0;
    sharedPlaneZ = (math::VEC2){ -50, 50 };

    LOG("System Alloced Size : %d \n", GetAllocedSizeFromStaticHeap());

    mInitialized = true;
}

void System::RemoveStripe(PtclStripe* stripe)
{
    stripe->data = NULL;

    if (stripeGroups[stripe->groupID] == stripe)
    {
        stripeGroups[stripe->groupID] = stripe->next;

        if (stripeGroups[stripe->groupID] != NULL)
            stripeGroups[stripe->groupID]->prev = NULL;
    }
    else
    {
        if (stripe->next != NULL)
            stripe->next->prev = stripe->prev;

        //if (stripe->prev != NULL) <-- No check, because... Nintendo
            stripe->prev->next = stripe->next;
    }
}

void System::RemoveStripe_()
{
    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        for (s32 j = 0; j < numStripeToRemove[i]; j++)
        {
            PtclStripe* stripe = stripesToRemove[i][j];

            RemoveStripe(stripe);

            stripesToRemove[i][j] = NULL;
        }

        numStripeToRemove[i] = 0;
    }
}

EmitterSet* System::RemoveEmitterSetFromDrawList(EmitterSet* emitterSet)
{
    EmitterSet* next = emitterSet->next;

    if (emitterSet == emitterSetGroupHead[emitterSet->groupID])
    {
        emitterSetGroupHead[emitterSet->groupID] = emitterSet->next;

        if (emitterSetGroupHead[emitterSet->groupID] != NULL)
            emitterSetGroupHead[emitterSet->groupID]->prev = NULL;

        if (emitterSet == emitterSetGroupTail[emitterSet->groupID])
            emitterSetGroupTail[emitterSet->groupID] = NULL;
    }
    else
    {
        if (emitterSet == emitterSetGroupTail[emitterSet->groupID])
            emitterSetGroupTail[emitterSet->groupID] = emitterSet->prev;

        if (emitterSet->next != NULL)
            emitterSet->next->prev = emitterSet->prev;

        if (emitterSet->prev != NULL)
            emitterSet->prev->next = emitterSet->next;

        else
            ERROR("EmitterSet Remove Failed.\n");
    }

    emitterSet->next = NULL;
    emitterSet->prev = NULL;

    return next;
}

void System::RemovePtcl()
{
    RemoveStripe_();

    for(u32 i = 0; i < 64u; i++)
        for (EmitterSet* emitterSet = emitterSetGroupHead[i]; emitterSet != NULL; )
            emitterSet = (emitterSet->numEmitter == 0) ? RemoveEmitterSetFromDrawList(emitterSet)
                                                       : emitterSet->next;
}

void System::AddStripeRemoveList(PtclStripe* stripe, CpuCore core)
{
    stripesToRemove[core][numStripeToRemove[core]++] = stripe;
}

void System::EmitChildParticle()
{
    for (u32 i = 0; i < CpuCore_Max; i++)
    {
        for (s32 j = 0; j < numChildParticle[i]; j++)
        {
            PtclInstance* ptcl = childParticles[i][j];
            EmitterComplexCalc::EmitChildParticle(ptcl->emitter, ptcl);
            childParticles[i][j] = NULL;
        }

        numChildParticle[i] = 0;
    }
}

void System::AddPtclAdditionList(PtclInstance* ptcl, CpuCore core)
{
    if (numChildParticle[core] > numParticleMax)
        return;

    childParticles[core][numChildParticle[core]] = ptcl;
    numChildParticle[core]++;
}

PtclStripe* System::AllocAndConnectStripe(EmitterInstance* emitter)
{
    s32 i = 0;
    do
    {
        currentStripeIdx++;
        if (currentStripeIdx >= numStripeMax) currentStripeIdx = 0;

        if (stripes[currentStripeIdx].data == NULL)
        {
            const SimpleEmitterData* data = emitter->data;

            PtclStripe* stripe = &stripes[currentStripeIdx];
            u8 groupID = emitter->groupID;

            if (stripeGroups[groupID] == NULL)
            {
                stripeGroups[groupID] = stripe;
                stripe->next = NULL;
                stripe->prev = NULL;
            }
            else
            {
                stripeGroups[groupID]->prev = stripe;
                stripe->next = stripeGroups[groupID];
                stripeGroups[groupID] = stripe;
                stripe->prev = NULL;
            }

            stripe->particle = NULL;
            stripe->queueFront = 0;
            stripe->queueRear  = 0;
            stripe->queueCount = 0;
            stripe->groupID = emitter->groupID;
            stripe->data = emitter->GetComplexEmitterData();
            stripe->counter = 0;
            stripe->queue[0].outer = math::VEC3::Zero();
            stripe->stripeUniformBlock = NULL;
            stripe->stripeUniformBlockCross = NULL;
            stripe->stripeVertexBuffer = NULL;

            stripe->texAnimParam[0].scroll.x = data->texAnimParam[0].texInitScroll.x - data->texAnimParam[0].texInitScrollRandom.x * emitter->random.GetF32Range(-1.0f, 1.0f);
            stripe->texAnimParam[0].scroll.y = data->texAnimParam[0].texInitScroll.y - data->texAnimParam[0].texInitScrollRandom.y * emitter->random.GetF32Range(-1.0f, 1.0f);
            stripe->texAnimParam[0].scale.x  = data->texAnimParam[0].texInitScale.x  - data->texAnimParam[0].texInitScaleRandom.x  * emitter->random.GetF32Range(-1.0f, 1.0f);
            stripe->texAnimParam[0].scale.y  = data->texAnimParam[0].texInitScale.y  - data->texAnimParam[0].texInitScaleRandom.y  * emitter->random.GetF32Range(-1.0f, 1.0f);
            stripe->texAnimParam[0].rotate   = data->texAnimParam[0].texInitRotate   - data->texAnimParam[0].texInitRotateRandom   * emitter->random.GetF32Range(-1.0f, 1.0f);

            stripe->texAnimParam[1].scroll.x = data->texAnimParam[1].texInitScroll.x - data->texAnimParam[1].texInitScrollRandom.x * emitter->random.GetF32Range(-1.0f, 1.0f);
            stripe->texAnimParam[1].scroll.y = data->texAnimParam[1].texInitScroll.y - data->texAnimParam[1].texInitScrollRandom.y * emitter->random.GetF32Range(-1.0f, 1.0f);
            stripe->texAnimParam[1].scale.x  = data->texAnimParam[1].texInitScale.x  - data->texAnimParam[1].texInitScaleRandom.x  * emitter->random.GetF32Range(-1.0f, 1.0f);
            stripe->texAnimParam[1].scale.y  = data->texAnimParam[1].texInitScale.y  - data->texAnimParam[1].texInitScaleRandom.y  * emitter->random.GetF32Range(-1.0f, 1.0f);
            stripe->texAnimParam[1].rotate   = data->texAnimParam[1].texInitRotate   - data->texAnimParam[1].texInitRotateRandom   * emitter->random.GetF32Range(-1.0f, 1.0f);

            stripe->texAnimParam[0].offset = (math::VEC2){ 0.0f, 0.0f };

            if (data->texAnimParam[0].texPtnAnimNum > 1) // TexPtnAnim Type Random
            {
                s32 texPtnAnimIdx = emitter->random.GetU32(data->texAnimParam[0].texPtnAnimNum);
                s32 texPtnAnimIdxDiv = data->texAnimParam[0].texPtnAnimIdxDiv[0];
                s32 offsetX = texPtnAnimIdx % texPtnAnimIdxDiv;
                s32 offsetY = texPtnAnimIdx / texPtnAnimIdxDiv;

                stripe->texAnimParam[0].offset.x = data->texAnimParam[0].uvScaleInit.x * (f32)offsetX;
                stripe->texAnimParam[0].offset.y = data->texAnimParam[0].uvScaleInit.y * (f32)offsetY;
            }

            stripe->texAnimParam[1].offset = (math::VEC2){ 0.0f, 0.0f };

            if (data->texAnimParam[1].texPtnAnimNum > 1) // TexPtnAnim Type Random
            {
                s32 texPtnAnimIdx = emitter->random.GetU32(data->texAnimParam[1].texPtnAnimNum);
                s32 texPtnAnimIdxDiv = data->texAnimParam[1].texPtnAnimIdxDiv[0];
                s32 offsetX = texPtnAnimIdx % texPtnAnimIdxDiv;
                s32 offsetY = texPtnAnimIdx / texPtnAnimIdxDiv;

                stripe->texAnimParam[1].offset.x = data->texAnimParam[1].uvScaleInit.x * (f32)offsetX;
                stripe->texAnimParam[1].offset.y = data->texAnimParam[1].uvScaleInit.y * (f32)offsetY;
            }

            stripe->flags = 0;

            return stripe;
        }
    } while (++i < numStripeMax);

    WARNING("StripeInstance is Empty.\n");

    return NULL;
}

PtclInstance* System::AllocPtcl()
{
    s32 i = 0;
    do
    {
        currentParticleIdx++;
        if (currentParticleIdx >= numParticleMax) currentParticleIdx = 0;

        if (particles[currentParticleIdx].data == NULL)
        {
            numEmittedParticle++;
            PtclInstance* ptcl = &particles[currentParticleIdx];
            return ptcl;
        }
    } while (++i < numParticleMax);

    WARNING("Particle is Empty.\n");

    return NULL;
}

void System::UpdateEmitterResInfo()
{
    for (u32 i = 0; i < 64u; i++)
        for (EmitterInstance* emitter = emitterGroups[i]; emitter != NULL; emitter = emitter->next)
            emitter->UpdateResInfo();
}

void System::UpdateEmitterSetResInfo()
{
    for (u32 i = 0; i < 64u; i++)
    {
        EmitterSet* emitterSet = emitterSetGroupHead[i];
        u32 flags = 0;

        for (; emitterSet != NULL; emitterSet = emitterSet->next)
            if (emitterSet->numEmitter > 0)
            {
                for (s32 j = 0; j < emitterSet->numEmitter; j++)
                    flags |= 1 << emitterSet->GetAliveEmitter(j)->data->_bitForUnusedFlag;

                emitterSet->_unusedFlags = flags;
            }
    }
}

EmitterSet* System::AllocEmitterSet(Handle* handle)
{
    EmitterSet* emitterSet = NULL;
    s32 i = 0;
    do
    {
        currentEmitterSetIdx++;
        if (currentEmitterSetIdx >= numEmitterSetMax) currentEmitterSetIdx = 0;

        if (emitterSets[currentEmitterSetIdx].numEmitter == 0)
        {
            emitterSet = &emitterSets[currentEmitterSetIdx];
            break;
        }
    } while (++i < numEmitterSetMax);

    handle->emitterSet = emitterSet;

    if (emitterSet == NULL)
    {
        WARNING("Emitter Set is Empty.\n");
        return NULL;
    }

    return emitterSet;
}

EmitterInstance* System::AllocEmitter(u8 groupID)
{
    EmitterInstance* emitter = NULL;
    s32 i = 0;
    do
    {
        currentEmitterIdx++;
        if (currentEmitterIdx >= numEmitterMax) currentEmitterIdx = 0;

        if (emitters[currentEmitterIdx].calc == NULL)
        {
            emitter = &emitters[currentEmitterIdx];
            break;
        }
    } while (++i < numEmitterMax);

    if (emitter == NULL)
    {
        WARNING("Emitter is Empty.\n");
        return NULL;
    }

    OSBlockSet(&emitter->numParticles, 0, sizeof(EmitterInstance) - ((u32)&emitter->numParticles - (u32)emitter));

    if (emitterGroups[groupID] == NULL)
    {
        emitterGroups[groupID] = emitter;
        emitter->next = NULL;
        emitter->prev = NULL;
    }
    else
    {
        emitterGroups[groupID]->prev = emitter;
        emitter->next = emitterGroups[groupID];
        emitterGroups[groupID] = emitter;
        emitter->prev = NULL;
    }

    numUnusedEmitters--;
    return emitter;
}

void System::AddEmitterSetToDrawList(EmitterSet* emitterSet, u8 groupID)
{
    if(emitterSetGroupHead[groupID] == NULL)
    {
        emitterSetGroupHead[groupID] = emitterSet;
        emitterSet->prev = NULL;
        emitterSet->next = NULL;
    }
    else
    {
        emitterSetGroupTail[groupID]->next = emitterSet;
        emitterSet->prev = emitterSetGroupTail[groupID];
        emitterSet->next = NULL;
    }

    emitterSetGroupTail[groupID] = emitterSet;
}

} } // namespace nw::eft
