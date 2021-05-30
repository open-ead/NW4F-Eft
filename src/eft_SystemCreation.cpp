#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_Handle.h>
#include <eft_Random.h>
#include <eft_ResData.h>
#include <eft_Resource.h>
#include <eft_System.h>

namespace nw { namespace eft {

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

    emitterSet->_1A0 = 0;
    emitterSet->_244 = 1.0f;
    emitterSet->_248 = 1.0f;
    emitterSet->_24C = 1.0f;
    emitterSet->_268 = 0;
    emitterSet->_278 = 0;
    emitterSet->doFade = 0;
    emitterSet->_289 = 0;
    emitterSet->noCalc = 0;
    emitterSet->noDraw = 0;
    emitterSet->_28C = 0;
    emitterSet->_28D = 0x80;

    emitterSet->_204    = (math::VEC3){ 1.0f, 1.0f, 1.0f };
    emitterSet->_210    = (math::VEC2){ 1.0f, 1.0f };
    emitterSet->_218    = (math::VEC2){ 1.0f, 1.0f };
    emitterSet->_220    = (math::VEC2){ 1.0f, 1.0f };
    emitterSet->_228    = (math::VEC3){ 1.0f, 1.0f, 1.0f };
    emitterSet->color.v = (math::VEC4){ 1.0f, 1.0f, 1.0f, 1.0f }; // 234
    emitterSet->_250    = (math::VEC3){ 0.0f, 0.0f, 0.0f };
    emitterSet->_26C    = (math::VEC3){ 0.0f, 0.0f, 0.0f };

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
        emitter->controller->_0 = 1.0f;
        emitter->controller->_4 = 1.0f;
        emitter->controller->_8 = 1.0f;
        emitter->controller->_C = 0x3F;

        emitter->groupID = groupID;

        emitterSet->_1A0 |= 1 << data->_29C;

        InitializeEmitter(emitter, data, resourceID, emitterSetID, seed, false);

        if (emitter->data->ptclMaxLifespan == 0x7FFFFFFF)
            emitterSet->_28C = 1;
    }

    emitterSet->numEmitterAtCreate = emitterSet->numEmitter;

    numCalcEmitterSet++;
    currentEmitterSetCreateID++;

    return true;
}

} } // namespace nw::eft
