#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_Handle.h>
#include <eft_Random.h>
#include <eft_ResData.h>
#include <eft_Resource.h>
#include <eft_Shader.h>
#include <eft_System.h>

namespace nw { namespace eft {

void System::InitializeEmitter(EmitterInstance* emitter, const SimpleEmitterData* data, u32 resourceID, s32 emitterSetID, u32 seed, bool keepCreateID)
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

    emitter->Init(data);
    emitter->calc = emitterCalc[emitter->data->type];
    emitter->controller->SetFollowType(data->ptclFollowType);

    {
        {
            VertexShaderKey vertexShaderKey;
            vertexShaderKey.Initialize(emitter->data);

            FragmentShaderKey fragmentShaderKey;
            fragmentShaderKey.Initialize(emitter->data);

            emitter->shader[ShaderType_Normal] = resources[resourceID]->GetShader(emitterSetID, &vertexShaderKey, &fragmentShaderKey);
        }

        if (strlen(emitter->data->userMacro1) != 0)
        {
            VertexShaderKey vertexShaderKey;
            vertexShaderKey.Initialize(emitter->data, emitter->data->userMacro1);

            FragmentShaderKey fragmentShaderKey;
            fragmentShaderKey.Initialize(emitter->data, emitter->data->userMacro1);

            emitter->shader[ShaderType_UserMacro1] = resources[resourceID]->GetShader(emitterSetID, &vertexShaderKey, &fragmentShaderKey);
        }

        if (strlen(emitter->data->userMacro2) != 0)
        {
            VertexShaderKey vertexShaderKey;
            vertexShaderKey.Initialize(emitter->data, emitter->data->userMacro2);

            FragmentShaderKey fragmentShaderKey;
            fragmentShaderKey.Initialize(emitter->data, emitter->data->userMacro2);

            emitter->shader[ShaderType_UserMacro2] = resources[resourceID]->GetShader(emitterSetID, &vertexShaderKey, &fragmentShaderKey);
        }
    }

    const ComplexEmitterData* cdata;

    if (emitter->data->type == EmitterType_Complex
        && (cdata = static_cast<const ComplexEmitterData*>(emitter->data), cdata->childFlags & 1))
    {
        const ChildData* childData = reinterpret_cast<const ChildData*>(cdata + 1);
        {
            VertexShaderKey vertexShaderKey;
            vertexShaderKey.Initialize(childData);

            FragmentShaderKey fragmentShaderKey;
            fragmentShaderKey.Initialize(childData, cdata->childFlags);

            emitter->childShader[ShaderType_Normal] = resources[resourceID]->GetShader(emitterSetID, &vertexShaderKey, &fragmentShaderKey);
        }

        if (strlen(childData->userMacro1) != 0)
        {
            VertexShaderKey vertexShaderKey;
            vertexShaderKey.Initialize(childData, childData->userMacro1);

            FragmentShaderKey fragmentShaderKey;
            fragmentShaderKey.Initialize(childData, cdata->childFlags, childData->userMacro1);

            emitter->childShader[ShaderType_UserMacro1] = resources[resourceID]->GetShader(emitterSetID, &vertexShaderKey, &fragmentShaderKey);
        }

        if (strlen(childData->userMacro2) != 0)
        {
            VertexShaderKey vertexShaderKey;
            vertexShaderKey.Initialize(childData, childData->userMacro2);

            FragmentShaderKey fragmentShaderKey;
            fragmentShaderKey.Initialize(childData, cdata->childFlags, childData->userMacro2);

            emitter->childShader[ShaderType_UserMacro2] = resources[resourceID]->GetShader(emitterSetID, &vertexShaderKey, &fragmentShaderKey);
        }
    }

    emitter->UpdateResInfo();

    emitter->primitive = NULL;
    if (emitter->data->meshType == MeshType_Primitive)
        emitter->primitive = resources[resourceID]->GetPrimitive(emitterSetID, emitter->data->primitiveIdx);

    emitter->childPrimitive = NULL;
    if (emitter->data->type == EmitterType_Complex
        && (cdata = static_cast<const ComplexEmitterData*>(emitter->data), cdata->childFlags & 1))
    {
        const ChildData* childData = reinterpret_cast<const ChildData*>(cdata + 1);
        if(childData->meshType == MeshType_Primitive)
            emitter->childPrimitive = resources[resourceID]->GetPrimitive(emitterSetID, childData->primitiveIdx);
    }
}

bool System::CreateEmitterSetID(Handle* handle, const math::MTX34& matrixSRT, s32 emitterSetID, u32 resourceID, u8 groupID, u32 emitterEnableMask)
{
    Random* gRandom = PtclRandom::GetGlobalRandom();

    s32 numEmitter = resources[resourceID]->emitterSets[emitterSetID].numEmitter;
    if (numEmitter > numUnusedEmitters)
        return false;

    EmitterSet* emitterSet = AllocEmitterSet(handle);
    if (emitterSet == NULL)
        return false;

    emitterSet->matrixSRT = matrixSRT;
    emitterSet->matrixRT  = matrixSRT;

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
    emitterSet->_unused = 0x80;

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

        emitterSet->emitters[emitterSet->numEmitter++] = emitter;
        emitter->emitterSet = emitterSet;

        emitter->controller = &emitterSet->controllers[i];
        emitter->controller->emitter = emitter;
        emitter->controller->emissionRatio = 1.0f;
        emitter->controller->emissionInterval = 1.0f;
        emitter->controller->life = 1.0f;
        emitter->controller->renderVisibilityFlags = 0x3F;

        emitter->groupID = groupID;

        emitterSet->_unusedFlags |= 1 << data->_bitForUnusedFlag;

        InitializeEmitter(emitter, data, resourceID, emitterSetID, seed, false);

        if (emitter->data->ptclMaxLifespan == 0x7FFFFFFF)
            emitterSet->infiniteLifespan = 1;
    }

    emitterSet->numEmitterAtCreate = emitterSet->numEmitter;

    numCalcEmitterSet++;
    currentEmitterSetCreateID++;

    return true;
}

} } // namespace nw::eft
