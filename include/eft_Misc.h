#ifndef EFT_MISC_H_
#define EFT_MISC_H_

#include <eft_Types.h>

namespace nw { namespace eft {

struct Heap;

// This function is supposed to print the error message and terminate the program
void OutputError(const char* fmt, ...);
#define ERROR OutputError

void SetStaticHeap(Heap* heap);
void* AllocFromStaticHeap(u32 size, u32 alignment = 0x80);
void SetDynamicHeap(Heap* heap);

// This function is just for printing warning messages
void OutputWarning(const char* fmt, ...);
#define WARNING OutputWarning

void* AllocFromDynamicHeap(u32 size, u32 alignment = 0x80);
void AddFreeListForDynamicHeap(void* ptr);
void FreeFromDynamicHeap(void* ptr, bool noDelay);

void InitializeDelayFreeList(u32 max);
void FlushDelayFreeList();

void SetSuppressOutputLog(bool suppressLog);

// This function is just for printing debug messages
void OutputLog(const char* fmt, ...);
#define LOG OutputLog

u32 GetAllocedSizeFromStaticHeap();

} } // namespace nw::eft

#endif // EFT_MISC_H_
