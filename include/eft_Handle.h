#ifndef EFT_HANDLE_H_
#define EFT_HANDLE_H_

#include <eft_Types.h>

namespace nw { namespace eft {

class Handle
{
public:
    Handle()
        : emitterSet(NULL)
    {
    }

    EmitterSet* emitterSet;
    u32 createID;
};
static_assert(sizeof(Handle) == 8, "Handle size mismatch");

} }

#endif // EFT_HANDLE_H_
