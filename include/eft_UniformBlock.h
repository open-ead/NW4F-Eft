#ifndef EFT_UNIFORM_BLOCK_H_
#define EFT_UNIFORM_BLOCK_H_

#include <types.h>

namespace nw { namespace eft {

struct EmitterStaticUniformBlock
{
    f32 uvScaleInit[4]; // VEC4
    f32 rotBasis[4]; // VEC4
    f32 shaderParam[4]; // VEC4

    f32 padding[0x10-3][4]; // Pad the block to 256 bytes for hardware needs
};

struct UniformBlock
{
    bool initialized;
    bool blockNotExist;
    u32 bufferSize;
    u32 location;
    u32 shaderStage; // 0 = Vertex, 1 = Fragment
};

} } // namespace nw::eft

#endif // EFT_UNIFORM_BLOCK_H_
