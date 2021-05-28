#ifndef EFT_RENDERER_H_
#define EFT_RENDERER_H_

#include <eft_RenderContext.h>

namespace nw { namespace eft {

class Heap;
class System;

class Renderer
{
public:
    Renderer(Heap* heap, System* system, const Config& config);
    ~Renderer();

    System* system;
    Heap* heap;
    RenderContext renderContext;
    u32 _28;
    u8 _2C[0x17C - 0x2C];
};

} } // namespace nw::eft

#endif // EFT_RENDERER_H_