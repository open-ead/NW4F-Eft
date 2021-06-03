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

CustomActionParticleRemoveCallback System::GetCurrentCustomActionParticleRemoveCallback(const EmitterInstance* emitter)
{
    if (_8A8 != -1 || currentCallbackID != CustomActionCallBackID_Invalid)
        return customActionParticleRemoveCallback[currentCallbackID];

    CustomActionCallBackID callbackID = emitter->data->callbackID;
    if (callbackID == CustomActionCallBackID_Invalid)
        return NULL;

    return customActionParticleRemoveCallback[callbackID];
}

CustomShaderEmitterPostCalcCallback System::GetCustomShaderEmitterPostCalcCallback(CustomShaderCallBackID callbackID)
{
    if (callbackID > CustomShaderCallBackID_Max) // No idea why not >=
        return NULL;

    return customShaderEmitterPostCalcCallback[callbackID];
}

} } // namespace nw::eft
