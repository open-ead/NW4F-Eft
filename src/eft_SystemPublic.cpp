#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_Heap.h>
#include <eft_Misc.h>
#include <eft_Particle.h>
#include <eft_Resource.h>
#include <eft_System.h>

#include <new>

namespace nw { namespace eft {

void System::ClearResource(Heap* heap, u32 resourceID)
{
    if (heap != NULL)
    {
        resources[resourceID]->Finalize(heap);
        heap->Free(resources[resourceID]);
    }

    else
    {
        Heap* resHeap = resources[resourceID]->heap;
        resources[resourceID]->Finalize(resHeap);
        resHeap->Free(resources[resourceID]);
    }

    resources[resourceID] = NULL;
}

void System::EntryResource(Heap* heap, void* resource, u32 resourceID, bool _unusedArg)
{
    if ((u32)resource & 0x7F)
        ERROR("resource memory must be 128 byte alignment.\n");

    if (resources[resourceID] != NULL)
        ClearResource(NULL, resourceID);

    resources[resourceID] = new (heap->Alloc(sizeof(Resource))) Resource(heap, resource, resourceID, this, _unusedArg);
}

void System::KillEmitter(EmitterInstance* emitter)
{
    if (emitter->calc == NULL)
        ERROR("Emitter Double Kill.\n");

    PtclInstance* ptcl;

    for (ptcl = emitter->particleHead; ptcl != NULL; ptcl = ptcl->next)
    {
        CustomActionParticleRemoveCallback callback = GetCurrentCustomActionParticleRemoveCallback(ptcl->emitter);
        if (callback != NULL)
        {
            ParticleRemoveArg arg = { .ptcl = ptcl };
            callback(arg);
        }

        ptcl->data = NULL;

        if (ptcl->complexParam != NULL && ptcl->complexParam->stripe != NULL)
            AddStripeRemoveList(ptcl->complexParam->stripe, CpuCore_1);
    }

    for (ptcl = emitter->childParticleHead; ptcl != NULL; ptcl = ptcl->next)
    {
        CustomActionParticleRemoveCallback callback = GetCurrentCustomActionParticleRemoveCallback(ptcl->emitter);
        if (callback != NULL)
        {
            ParticleRemoveArg arg = { .ptcl = ptcl };
            callback(arg);
        }

        ptcl->data = NULL;
    }

    if (emitter == emitterGroups[emitter->groupID])
    {
        emitterGroups[emitter->groupID] = emitter->next;

        if (emitterGroups[emitter->groupID] != NULL)
            emitterGroups[emitter->groupID]->prev = NULL;
    }
    else
    {
        if (emitter->next != NULL)
            emitter->next->prev = emitter->prev;

        //if (emitter->prev != NULL) <-- No check, because... Nintendo
            emitter->prev->next = emitter->next;
    }

    CustomShaderCallBackID callbackID = static_cast<CustomShaderCallBackID>(emitter->data->shaderUserSetting);
    if (GetCustomShaderEmitterFinalizeCallback(callbackID) != NULL)
    {
        ShaderEmitterFinalizeArg arg = { .emitter = emitter };
        GetCustomShaderEmitterFinalizeCallback(callbackID)(arg);
    }

    if (emitter->numPtclAttributeBufferGpuMax != 0 && emitter->ptclAttributeBufferGpu != NULL)
    {
        FreeFromDynamicHeap(emitter->ptclAttributeBufferGpu, false);
        emitter->ptclAttributeBufferGpu = NULL;
        emitter->numPtclAttributeBufferGpuMax = 0;

        emitter->posStreamOutAttributeBuffer.Finalize();
        emitter->vecStreamOutAttributeBuffer.Finalize();
    }

    RemoveStripe_();

    if (emitter->emitterSet != NULL) emitter->emitterSet->numEmitter--;
    emitter->emitterSetCreateID = 0xFFFFFFFF;
    emitter->calc = NULL;

    numUnusedEmitters++;
    if(emitter->emitterSet->numEmitter == 0)
        numCalcEmitterSet--;
}

void System::KillEmitterGroup(u8 groupID)
{
    for (EmitterInstance* emitter = emitterGroups[groupID]; emitter != NULL; emitter = emitter->next)
        KillEmitter(emitter);

    emitterSetGroupHead[groupID] = NULL;
    emitterSetGroupTail[groupID] = NULL;
}

void System::KillEmitterSet(EmitterSet* emitterSet)
{
    if (!(emitterSet->numEmitter > 0))
        return;

    RemoveEmitterSetFromDrawList(emitterSet);

    for (s32 i = 0; i < emitterSet->numEmitterAtCreate; i++)
        if (emitterSet->emitters[i]->emitterSetCreateID == emitterSet->createID
            && emitterSet->emitters[i]->calc != NULL)
        {
            KillEmitter(emitterSet->emitters[i]);
        }
}

} } // namespace nw::eft
