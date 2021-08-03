#include <eft_Emitter.h>
#include <eft_ResData.h>
#include <eft_System.h>

namespace nw { namespace eft {

CustomActionEmitterMatrixSetCallback System::GetCurrentCustomActionEmitterMatrixSetCallback(const EmitterInstance* emitter)
{
    if (currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionEmitterMatrixSetCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    return customActionEmitterMatrixSetCallback[callbackID];
}

CustomActionEmitterPreCalcCallback System::GetCurrentCustomActionEmitterPreCalcCallback(const EmitterInstance* emitter)
{
    if (currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionEmitterPreCalcCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    return customActionEmitterPreCalcCallback[callbackID];
}

CustomActionEmitterPostCalcCallback System::GetCurrentCustomActionEmitterPostCalcCallback(const EmitterInstance* emitter)
{
    if (currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionEmitterPostCalcCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    return customActionEmitterPostCalcCallback[callbackID];
}

CustomActionParticleEmitCallback System::GetCurrentCustomActionParticleEmitCallback(const EmitterInstance* emitter)
{
    if (currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionParticleEmitCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    return customActionParticleEmitCallback[callbackID];
}

CustomActionParticleRemoveCallback System::GetCurrentCustomActionParticleRemoveCallback(const EmitterInstance* emitter)
{
    if (currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionParticleRemoveCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    return customActionParticleRemoveCallback[callbackID];
}

CustomActionParticleCalcCallback System::GetCurrentCustomActionParticleCalcCallback(const EmitterInstance* emitter)
{
    if (currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionParticleCalcCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    return customActionParticleCalcCallback[callbackID];
}

CustomActionParticleMakeAttributeCallback System::GetCurrentCustomActionParticleMakeAttributeCallback(const EmitterInstance* emitter)
{
    if (currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionParticleMakeAttributeCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    return customActionParticleMakeAttributeCallback[callbackID];
}

CustomActionEmitterDrawOverrideCallback System::GetCurrentCustomActionEmitterDrawOverrideCallback(const EmitterInstance* emitter)
{
    if (currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionEmitterDrawOverrideCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    return customActionEmitterDrawOverrideCallback[callbackID];
}

CustomShaderEmitterInitializeCallback System::GetCustomShaderEmitterInitializeCallback(CustomShaderCallBackID callbackID)
{
    if (callbackID > CustomShaderCallBackID_Max) // No idea why not >=
        return NULL;

    return customShaderEmitterInitializeCallback[callbackID];
}

CustomShaderEmitterFinalizeCallback System::GetCustomShaderEmitterFinalizeCallback(CustomShaderCallBackID callbackID)
{
    if (callbackID > CustomShaderCallBackID_Max) // No idea why not >=
        return NULL;

    return customShaderEmitterFinalizeCallback[callbackID];
}

CustomShaderEmitterPreCalcCallback System::GetCustomShaderEmitterPreCalcCallback(CustomShaderCallBackID callbackID)
{
    if (callbackID > CustomShaderCallBackID_Max) // No idea why not >=
        return NULL;

    return customShaderEmitterPreCalcCallback[callbackID];
}

CustomShaderEmitterPostCalcCallback System::GetCustomShaderEmitterPostCalcCallback(CustomShaderCallBackID callbackID)
{
    if (callbackID > CustomShaderCallBackID_Max) // No idea why not >=
        return NULL;

    return customShaderEmitterPostCalcCallback[callbackID];
}

CustomShaderDrawOverrideCallback System::GetCustomShaderDrawOverrideCallback(CustomShaderCallBackID callbackID)
{
    if (callbackID > CustomShaderCallBackID_Max) // No idea why not >=
        return NULL;

    return customShaderDrawOverrideCallback[callbackID];
}

CustomShaderRenderStateSetCallback System::GetCustomShaderRenderStateSetCallback(CustomShaderCallBackID callbackID)
{
    if (callbackID > CustomShaderCallBackID_Max) // No idea why not >=
        return NULL;

    return customShaderRenderStateSetCallback[callbackID];
}

DrawPathRenderStateSetCallback System::GetDrawPathRenderStateSetCallback(DrawPathFlag flag)
{
    if (flag != 0)
    {
        for (u32 i = 0; i < DrawPathCallback_Max; i++)
            if (flag & drawPathCallbackFlags[i])
                return drawPathRenderStateSetCallback[i];
    }

    return NULL;
}

} } // namespace nw::eft
