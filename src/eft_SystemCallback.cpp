#include <eft_Emitter.h>
#include <eft_ResData.h>
#include <eft_System.h>

namespace nw { namespace eft {

CustomActionEmitterPreCalcCallback System::GetCurrentCustomActionEmitterPreCalcCallback(const EmitterInstance* emitter)
{
    if (_8A8 != -1 || currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionEmitterPreCalcCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    if (callbackID == CustomActionCallBackID_Invalid)
        return NULL;

    return customActionEmitterPreCalcCallback[callbackID];
}

CustomActionEmitterPostCalcCallback System::GetCurrentCustomActionEmitterPostCalcCallback(const EmitterInstance* emitter)
{
    if (_8A8 != -1 || currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionEmitterPostCalcCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    if (callbackID == CustomActionCallBackID_Invalid)
        return NULL;

    return customActionEmitterPostCalcCallback[callbackID];
}

CustomActionParticleEmitCallback System::GetCurrentCustomActionParticleEmitCallback(const EmitterInstance* emitter)
{
    if (_8A8 != -1 || currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionParticleEmitCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    if (callbackID == CustomActionCallBackID_Invalid)
        return NULL;

    return customActionParticleEmitCallback[callbackID];
}

CustomActionParticleRemoveCallback System::GetCurrentCustomActionParticleRemoveCallback(const EmitterInstance* emitter)
{
    if (_8A8 != -1 || currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionParticleRemoveCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    if (callbackID == CustomActionCallBackID_Invalid)
        return NULL;

    return customActionParticleRemoveCallback[callbackID];
}

CustomActionEmitterDrawOverrideCallback System::GetCurrentCustomActionEmitterDrawOverrideCallback(const EmitterInstance* emitter)
{
    if (_8A8 != -1 || currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionEmitterDrawOverrideCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    if (callbackID == CustomActionCallBackID_Invalid)
        return NULL;

    return customActionEmitterDrawOverrideCallback[callbackID];
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

} } // namespace nw::eft
