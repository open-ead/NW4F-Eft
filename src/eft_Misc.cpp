#include <eft_Heap.h>
#include <eft_Misc.h>

#include <cstring>

namespace nw { namespace eft {

u32 g_StaticHeapAllocedSize = 0;
Heap* g_StaticHeap = NULL;
Heap* g_DynamicHeap = NULL;

void** g_DelayFreeList[3];
u32 g_DelayFreeListNum[3];
u32 g_DelayFreeListMax = 0;
u32 g_DelayFreeListCntr = 0;

bool g_SuppressLog = false;

void OutputError(const char* fmt, ...)
{
    // This function is supposed to print the error message and terminate the program
}

void SetStaticHeap(Heap* heap)
{
    g_StaticHeap = heap;
}

void* AllocFromStaticHeap(u32 size, u32 alignment)
{
    void* ptr = g_StaticHeap->Alloc(size, alignment);
    g_StaticHeapAllocedSize += size;
    return ptr;
}

void SetDynamicHeap(Heap* heap)
{
    g_DynamicHeap = heap;
}

void OutputWarning(const char* fmt, ...)
{
    // This function is just for printing warning messages
}

void* AllocFromDynamicHeap(u32 size, u32 alignment)
{
    return g_DynamicHeap->Alloc(size + (0x100 - 1) & ~(0x100 - 1), alignment);
}

void AddFreeListForDynamicHeap(void* ptr)
{
    g_DelayFreeList[g_DelayFreeListCntr][g_DelayFreeListNum[g_DelayFreeListCntr]++] = ptr;
}

void FreeFromDynamicHeap(void* ptr, bool noDelay)
{
    if (noDelay)
        return g_DynamicHeap->Free(ptr);

    AddFreeListForDynamicHeap(ptr);
}

void InitializeDelayFreeList(u32 max)
{
    g_DelayFreeListMax = max;

    for (u32 i = 0; i < 3; i++)
    {
        g_DelayFreeList[i] = static_cast<void**>(AllocFromStaticHeap(sizeof(void*) * g_DelayFreeListMax));
        memset(g_DelayFreeList[i], 0, sizeof(void*) * g_DelayFreeListMax);
        g_DelayFreeListNum[i] = 0;
    }
}

void FlushDelayFreeList()
{
    if (++g_DelayFreeListCntr == 3)
        g_DelayFreeListCntr = 0;

    u32 i = 0;
    for (; g_DelayFreeList[g_DelayFreeListCntr][i] != NULL; i++)
        g_DynamicHeap->Free(g_DelayFreeList[g_DelayFreeListCntr][i]);

    memset(g_DelayFreeList[g_DelayFreeListCntr], 0, sizeof(void*) * i);
    g_DelayFreeListNum[g_DelayFreeListCntr] = 0;
}

void SetSuppressOutputLog(bool suppressLog)
{
    g_SuppressLog = suppressLog;
}

void OutputLog(const char* fmt, ...)
{
    // This function is just for printing debug messages
}

u32 GetAllocedSizeFromStaticHeap()
{
    return g_StaticHeapAllocedSize;
}

} } // namespace nw::eft
