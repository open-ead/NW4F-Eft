#ifndef EFT_EMITTER_FIELD_CURL_NOISE_H_
#define EFT_EMITTER_FIELD_CURL_NOISE_H_

#define INCLUDE_CAFE
#include <eft_Types.h>

namespace nw { namespace eft {

class CurlNoiseTbl
{
public:
    math::VEC3 GetVec3(s32 z, s32 y, s32 x)
    {
        if (z < 0) z = -z;
        if (y < 0) y = -y;
        if (x < 0) x = -x;

        z %= depth;
        y %= height;
        x %= width;

        u32 pos = z * height * width + y * width + x;

        return (math::VEC3){
            data[pos][0] / 127.0f,
            data[pos][1] / 127.0f,
            data[pos][2] / 127.0f,
        };
    }

    union
    {
        u32 width;
        u32 height;
        u32 depth;
        u32 length;
    };
    u8 (*data)[3];
};
static_assert(sizeof(CurlNoiseTbl) == 8, "CurlNoiseTbl size mismatch");

class CurlNoiseTexture
{
public:
    void Initialize();

    GX2Texture gx2Texture;
    void* imagePtr;
};

void InitializeCurlNoise();
const GX2Texture* GetCurlNoiseTexture();

} } // namespace nw::eft

#endif // EFT_EMITTER_FIELD_CURL_NOISE_H_
