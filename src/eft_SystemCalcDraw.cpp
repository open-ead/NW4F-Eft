#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_Misc.h>
#include <eft_Renderer.h>
#include <eft_ResData.h>
#include <eft_Shader.h>
#include <eft_System.h>

#include <cstring>

namespace nw { namespace eft {

void System::BeginFrame()
{
    numCalcEmitter = 0;
    numCalcParticle = 0;
    numCalcParticleGpu = 0;
    numCalcStripe = 0;
    numEmittedParticle = 0;
    activeGroupsFlg = 0;

    counter++;
    _unused = 0;
    streamOutEmitterHead = NULL;

    memset(_unusedFlags, 0, CpuCore_Max * 64 * sizeof(u32));

    RemoveStripe_();
    FlushDelayFreeList();
}

void System::SwapDoubleBuffer()
{
    for (u32 i = 0; i < CpuCore_Max; i++)
        renderers[i]->SwapDoubleBuffer();

    doubleBufferSwapped = 1;
}

void System::CalcEmitter(u8 groupID, f32 emissionSpeed)
{
    activeGroupsFlg |= 1ULL << groupID;
    for (EmitterInstance* emitter = emitterGroups[groupID]; emitter != NULL; emitter = emitter->next)
        CalcEmitter(emitter, emissionSpeed);
}

void System::CalcParticle(EmitterInstance* emitter, CpuCore core)
{
    if (emitter == NULL || emitter->calc == NULL)
        return;

    CustomShaderEmitterPostCalcCallback callback = GetCustomShaderEmitterPostCalcCallback(static_cast<CustomShaderCallBackID>(emitter->data->shaderUserSetting));

    bool noCalcBehavior = false;
    if ((activeGroupsFlg & (1ULL << emitter->groupID)) == 0)
        noCalcBehavior = true;
    if (emitter->emitterSet->noCalc != 0)
        noCalcBehavior = true;

    if (callback != NULL)
    {
        ShaderEmitterPostCalcArg arg = {
            .emitter = emitter,
            .noCalcBehavior = noCalcBehavior,
            .childParticle = false,
        };
        callback(arg);
    }

    u32 numCalc = emitter->calc->CalcParticle(emitter, core, noCalcBehavior, false);
    numCalcParticle += numCalc;

    if (emitter->shader[ShaderType_Normal] != NULL && emitter->shader[ShaderType_Normal]->vertexShaderKey.flags[0] & 0x2000000)
        numCalcParticleGpu += numCalc;

    if (emitter->data->vertexTransformMode == VertexTransformMode_Stripe)
        numCalcStripe += numCalc;

    if (emitter->data->vertexTransformMode == VertexTransformMode_Complex_Stripe)
        numCalcStripe++;

    _unusedFlags[core][emitter->groupID] |= 1 << emitter->data->_bitForUnusedFlag;
}

void System::CalcChildParticle(EmitterInstance* emitter, CpuCore core)
{
    if (emitter == NULL || emitter->calc == NULL || emitter->data->type != EmitterType_Complex)
        return;

    bool noCalcBehavior = false;
    if ((activeGroupsFlg & (1ULL << emitter->groupID)) == 0)
        noCalcBehavior = true;
    if (emitter->emitterSet->noCalc != 0)
        noCalcBehavior = true;

    if (emitter->HasChild())
    {
        CustomShaderEmitterPostCalcCallback callback = GetCustomShaderEmitterPostCalcCallback(static_cast<CustomShaderCallBackID>(emitter->GetChildData()->shaderUserSetting));
        if (callback != NULL)
        {
            ShaderEmitterPostCalcArg arg = {
                .emitter = emitter,
                .noCalcBehavior = noCalcBehavior,
                .childParticle = true,
            };
            callback(arg);
        }
    }

    if (emitter->HasChild())
        numCalcParticle += emitter->calc->CalcChildParticle(emitter, core, noCalcBehavior, false);
}

void System::FlushCache()
{
    for (u32 i = 0; i < CpuCore_Max; i++)
        renderers[i]->FlushCache();
}

void System::FlushGpuCache()
{
    GX2Invalidate(static_cast<GX2InvalidateType>(GX2_INVALIDATE_ATTRIB_BUFFER
                                                 | GX2_INVALIDATE_TEXTURE
                                                 | GX2_INVALIDATE_UNIFORM_BLOCK
                                                 | GX2_INVALIDATE_SHADER), NULL, 0xFFFFFFFF);
}

void System::CalcEmitter(EmitterInstance* emitter, f32 emissionSpeed)
{
    if (emitter->emitterSet->noCalc == 0 && activeGroupsFlg & (1ULL << emitter->groupID))
    {
        if (emitter->emissionSpeed != emissionSpeed)
        {
            emitter->preCalcCounter = emitter->counter;
            emitter->emissionSpeed = emissionSpeed;
        }

        if (GetCurrentCustomActionEmitterPreCalcCallback(emitter) != NULL)
        {
            EmitterPreCalcArg arg = { .emitter = emitter };
            GetCurrentCustomActionEmitterPreCalcCallback(emitter)(arg);
        }
        else if (GetCustomShaderEmitterPreCalcCallback(static_cast<CustomShaderCallBackID>(emitter->data->shaderUserSetting)) != NULL)
        {
            ShaderEmitterPreCalcArg arg = { .emitter = emitter };
            GetCustomShaderEmitterPreCalcCallback(static_cast<CustomShaderCallBackID>(emitter->data->shaderUserSetting))(arg);
        }
        else
        {
            emitter->calc->CalcEmitter(emitter);
        }

        if (GetCurrentCustomActionEmitterPostCalcCallback(emitter) != NULL)
        {
            EmitterPostCalcArg arg = { .emitter = emitter };
            GetCurrentCustomActionEmitterPostCalcCallback(emitter)(arg);
        }

        if (emitter->shader[ShaderType_Normal]->vertexShaderKey.flags[0] & 0x8000000)
        {
            if (streamOutEmitterHead == NULL)
                streamOutEmitterTail = streamOutEmitterHead = emitter;

            else
                streamOutEmitterTail = (streamOutEmitterTail->nextStreamOut = emitter);

            emitter->nextStreamOut = NULL;
        }

        numCalcEmitter++;
    }
}

void System::CalcParticle(bool flushCache)
{
    for (u32 i = 0; i < 64u; i++)
        for (EmitterInstance* emitter = emitterGroups[i]; emitter != NULL; emitter = emitter->next)
        {
            CalcParticle(emitter, CpuCore_1);
            _unusedFlags[CpuCore_1][emitter->groupID] |= 1 << emitter->data->_bitForUnusedFlag;

            if (emitter->data->type == EmitterType_Complex
                && (static_cast<const ComplexEmitterData*>(emitter->data)->childFlags & 1))
            {
                EmitChildParticle();
                CalcChildParticle(emitter, CpuCore_1);
            }
        }

    RemovePtcl();

    if (flushCache)
    {
        FlushCache();
        FlushGpuCache();
    }
}

void System::BeginRender(const math::MTX44& proj, const math::MTX34& view, const math::VEC3& cameraWorldPos, f32 zNear, f32 zFar)
{
    this->view[OSGetCoreId()] = math::MTX44(view);
    renderers[OSGetCoreId()]->BeginRender(proj, view, cameraWorldPos, zNear, zFar);
}

void System::RenderEmitter(const EmitterInstance* emitter, void* argData)
{
    if (emitter == NULL)
        return;

    CpuCore core = static_cast<CpuCore>(OSGetCoreId());

    if (GetCurrentCustomActionEmitterDrawOverrideCallback(emitter) != NULL)
    {
        EmitterDrawOverrideArg arg = {
            .emitter = emitter,
            .renderer = renderers[core],
            .flushCache = true,
            .argData = argData,
        };
        GetCurrentCustomActionEmitterDrawOverrideCallback(emitter)(arg);
    }
    else
    {
        renderers[core]->EntryParticle(emitter, argData);
    }
}

void System::EndRender()
{
    renderers[OSGetCoreId()]->EndRender();
}

void System::SwapStreamOutBuffer()
{
    for (EmitterInstance* emitter = streamOutEmitterHead; emitter != NULL; emitter = emitter->nextStreamOut)
        emitter->swapStreamOut ^= 1;
}

void System::CalcStreamOutEmittter()
{
    if (streamOutEmitterHead == NULL)
        return;

    renderers[OSGetCoreId()]->BeginStremOut();

    for (EmitterInstance* emitter = streamOutEmitterHead; emitter != NULL; emitter = emitter->nextStreamOut)
        renderers[OSGetCoreId()]->CalcStremOutParticle(emitter, true);

    renderers[OSGetCoreId()]->EndStremOut();
}

} } // namespace nw::eft
