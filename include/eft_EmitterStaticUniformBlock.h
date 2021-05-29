#ifndef EFT_EMITTER_STATIC_UNIFORM_BLOCK_H_
#define EFT_EMITTER_STATIC_UNIFORM_BLOCK_H_

#include <types.h>

namespace nw { namespace eft {

struct EmitterStaticUniformBlock
{
    f32 uvScaleInit[4]; // VEC4
    f32 rotBasis[4]; // VEC4
    f32 shaderParam[4]; // VEC4

    f32 padding[0x10-3][4]; // Pad the block to 256 bytes for hardware needs
};

} } // namespace nw::eft

#endif // EFT_EMITTER_STATIC_UNIFORM_BLOCK_H_
