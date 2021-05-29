#include <eft_Heap.h>
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

void System::EntryResource(Heap* heap, void* resource, u32 resourceID)
{
    if (resources[resourceID] != NULL)
        ClearResource(NULL, resourceID);

    resources[resourceID] = new (heap->Alloc(sizeof(Resource))) Resource(heap, resource, resourceID, this);
}

} } // namespace nw::eft
