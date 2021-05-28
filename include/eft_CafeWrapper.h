#ifndef EFT_CAFE_WRAPPER_H_
#define EFT_CAFE_WRAPPER_H_

#include <cafe.h>

namespace nw { namespace eft {

class TextureSampler
{
public:
    TextureSampler();
    ~TextureSampler();

    GX2Sampler sampler;
};

} } // namespace nw::eft

#endif // EFT_CAFE_WRAPPER_H_
