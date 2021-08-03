#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_Handle.h>
#include <eft_Misc.h>
#include <eft_Random.h>
#include <eft_ResData.h>
#include <eft_Resource.h>
#include <eft_Shader.h>
#include <eft_System.h>

namespace nw { namespace eft {

void System::InitializeEmitter(EmitterInstance* emitter, const SimpleEmitterData* data, EmitterStaticUniformBlock* esub, EmitterStaticUniformBlock* cesub, u32 resourceID, s32 emitterSetID, u32 seed, bool keepCreateID)
{
    Random* gRandom = PtclRandom::GetGlobalRandom();

    const u32 data_seed = data->seed;
    if (data_seed == 0xFFFFFFFF)
        emitter->random.Init(seed);
    else if (data_seed == 0)
        emitter->random.Init(gRandom->GetU32());
    else
        emitter->random.Init(data_seed);

    if (!keepCreateID)
        emitter->emitterSetCreateID = currentEmitterSetCreateID;

    emitter->emitterStaticUniformBlock = esub;
    emitter->childEmitterStaticUniformBlock = cesub;

    emitter->Init(data);

    if (data->emitFunction == 15)
        emitter->volumePrimitive = resources[resourceID]->GetPrimitive(emitterSetID, data->volumePrimitive.idx);

    else
        emitter->volumePrimitive = NULL;

    const ChildData* childData = emitter->GetChildData();

    {
        emitter->shader[ShaderType_Normal] = resources[resourceID]->GetShader(emitterSetID, emitter->data->shaderIdx);

        emitter->shader[ShaderType_UserMacro1] = NULL;
        if (emitter->data->userShaderIdx1 != 0)
            emitter->shader[ShaderType_UserMacro1] = resources[resourceID]->GetShader(emitterSetID, emitter->data->userShaderIdx1);

        emitter->shader[ShaderType_UserMacro2] = NULL;
        if (emitter->data->userShaderIdx2 != 0)
            emitter->shader[ShaderType_UserMacro2] = resources[resourceID]->GetShader(emitterSetID, emitter->data->userShaderIdx2);
    }

    if (childData != NULL)
    {
        emitter->childShader[ShaderType_Normal] = resources[resourceID]->GetShader(emitterSetID, childData->shaderIdx);

        emitter->childShader[ShaderType_UserMacro1] = NULL;
        if (childData->userShaderIdx1 != 0)
            emitter->childShader[ShaderType_UserMacro1] = resources[resourceID]->GetShader(emitterSetID, childData->userShaderIdx1);

        emitter->childShader[ShaderType_UserMacro2] = NULL;
        if (childData->userShaderIdx2 != 0)
            emitter->childShader[ShaderType_UserMacro2] = resources[resourceID]->GetShader(emitterSetID, childData->userShaderIdx2);
    }

    if (emitter->shader[ShaderType_Normal]->vertexShaderKey.flags[0] & 0x2000000)
        emitter->calc = emitterCalc[EmitterType_SimpleGpu];

    else
        emitter->calc = emitterCalc[emitter->data->type];

    emitter->primitive = NULL;
    if (emitter->data->meshType == MeshType_Primitive)
        emitter->primitive = resources[resourceID]->GetPrimitive(emitterSetID, emitter->data->primitive.idx);

    emitter->childPrimitive = NULL;
    if (childData != NULL)
    {
        if(childData->meshType == MeshType_Primitive)
            emitter->childPrimitive = resources[resourceID]->GetPrimitive(emitterSetID, childData->primitive.idx);
    }

    emitter->UpdateResInfo();
    emitter->userData = 0;

    CustomShaderCallBackID callbackID = static_cast<CustomShaderCallBackID>(data->shaderUserSetting);
    if (GetCustomShaderEmitterInitializeCallback(callbackID) != NULL)
    {
        ShaderEmitterInitializeArg arg = { .emitter = emitter };
        GetCustomShaderEmitterInitializeCallback(callbackID)(arg);
    }
}

bool System::CreateEmitterSetID(Handle* handle, const math::MTX34& matrixRT, s32 emitterSetID, u32 resourceID, u8 groupID, u32 emitterEnableMask)
{
    if (resourceID >= numResourceMax || resources[resourceID] == NULL || resources[resourceID]->resource->numEmitterSet <= emitterSetID)
        return false;

    Random* gRandom = PtclRandom::GetGlobalRandom();

    s32 numEmitter = resources[resourceID]->emitterSets[emitterSetID].numEmitter;
    if (numEmitter > numUnusedEmitters)
    {
        WARNING("Emitter is Empty.\n");
        return false;
    }

    EmitterSet* emitterSet = AllocEmitterSet(handle);
    if (emitterSet == NULL)
        return false;

    emitterSet->matrixSRT = matrixRT;
    emitterSet->matrixRT  = matrixRT;

    emitterSet->_unusedFlags = 0;
    emitterSet->allDirVel = 1.0f;
    emitterSet->dirVel = 1.0f;
    emitterSet->dirVelRandom = 1.0f;
    emitterSet->startFrame = 0;
    emitterSet->numEmissionPoints = 0;
    emitterSet->doFade = 0;
    emitterSet->dirSet = 0;
    emitterSet->noCalc = 0;
    emitterSet->noDraw = 0;
    emitterSet->infiniteLifespan = 0;
    emitterSet->renderPriority = 0x80;

    emitterSet->scaleForMatrix     = (math::VEC3){ 1.0f, 1.0f, 1.0f };
    emitterSet->ptclScale          = (math::VEC2){ 1.0f, 1.0f };
    emitterSet->ptclEmitScale      = (math::VEC2){ 1.0f, 1.0f };
    emitterSet->ptclEffectiveScale = (math::VEC2){ 1.0f, 1.0f };
    emitterSet->emitterVolumeScale = (math::VEC3){ 1.0f, 1.0f, 1.0f };
    emitterSet->color.v            = (math::VEC4){ 1.0f, 1.0f, 1.0f, 1.0f };
    emitterSet->addVelocity        = (math::VEC3){ 0.0f, 0.0f, 0.0f };
    emitterSet->ptclRotate         = (math::VEC3){ 0.0f, 0.0f, 0.0f };

    for (u32 i = 0; i < 16u; i++)
        emitterSet->emitters[i] = NULL;

    emitterSet->createID = currentEmitterSetCreateID;
    emitterSet->resourceID = resourceID;
    emitterSet->emitterSetID = emitterSetID;
    emitterSet->next = NULL;
    emitterSet->prev = NULL;
    emitterSet->groupID = groupID;
    emitterSet->userData = 0;

    AddEmitterSetToDrawList(emitterSet, groupID);

    u32 seed = gRandom->GetU32();

    handle->createID = currentEmitterSetCreateID;

    for (s32 i = numEmitter - 1; i >= 0; i--)
    {
        if (!(emitterEnableMask & 1 << i))
            continue;

        EmitterInstance* emitter = AllocEmitter(groupID);
        if (emitter == NULL)
            break;

        const SimpleEmitterData* data = static_cast<const SimpleEmitterData*>(resources[resourceID]->emitterSets[emitterSetID].emitterRef[i].data);
        EmitterStaticUniformBlock* emitterStaticUniformBlock                = resources[resourceID]->emitterSets[emitterSetID].emitterRef[i].emitterStaticUniformBlock;
        EmitterStaticUniformBlock* childEmitterStaticUniformBlock           = resources[resourceID]->emitterSets[emitterSetID].emitterRef[i].childEmitterStaticUniformBlock;

        emitterSet->emitters[emitterSet->numEmitter++] = emitter;
        emitter->emitterSet = emitterSet;

        emitter->controller = &emitterSet->controllers[i];
        emitter->controller->emitter = emitter;
        emitter->controller->emissionRatioChanged = false;
        emitter->controller->emissionRatio = 1.0f;
        emitter->controller->emissionInterval = 1.0f;
        emitter->controller->life = 1.0f;
        emitter->controller->renderVisibilityFlags = 0x3F;

        emitter->groupID = groupID;

        emitterSet->_unusedFlags |= 1 << data->_bitForUnusedFlag;

        InitializeEmitter(emitter, data, emitterStaticUniformBlock, childEmitterStaticUniformBlock, resourceID, emitterSetID, seed, false);

        if (emitter->data->ptclMaxLifespan == 0x7FFFFFFF)
            emitterSet->infiniteLifespan = 1;
    }

    emitterSet->numEmitterAtCreate = emitterSet->numEmitter;

    numCalcEmitterSet++;
    currentEmitterSetCreateID++;

    return true;
}

} } // namespace nw::eft
