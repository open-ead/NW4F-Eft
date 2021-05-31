#ifndef EFT_UNIFORM_BLOCK_H_
#define EFT_UNIFORM_BLOCK_H_

#include <math/math_VEC4.h>
#include <eft_Types.h>

namespace nw { namespace eft {

struct EmitterStaticUniformBlock
{
    math::VEC4 uvScaleInit;
    math::VEC4 rotBasis;
    math::VEC4 shaderParam;

    math::VEC4 pad[0x10-3]; // Pad the block to 256 bytes for hardware needs
};
static_assert(sizeof(EmitterStaticUniformBlock) == 0x100, "EmitterStaticUniformBlock size mismatch");

struct UniformBlock
{
    bool initialized;
    bool blockNotExist;
    u32 bufferSize;
    u32 location;
    u32 shaderStage; // 0 = Vertex, 1 = Fragment
};
static_assert(sizeof(UniformBlock) == 0x10, "UniformBlock size mismatch");

} } // namespace nw::eft

#endif // EFT_UNIFORM_BLOCK_H_
