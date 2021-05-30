#ifndef EFT_HANDLE_H_
#define EFT_HANDLE_H_

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

} }

#endif // EFT_HANDLE_H_
