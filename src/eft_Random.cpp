#include <eft_Misc.h>
#include <eft_Random.h>

namespace nw { namespace eft {

Random PtclRandom::gRandom;
math::VEC3* PtclRandom::mVec3Tbl = NULL;
math::VEC3* PtclRandom::mNormalizedVec3Tbl = NULL;

Random* PtclRandom::GetGlobalRandom()
{
    return &PtclRandom::gRandom;
}

void PtclRandom::Initialize()
{
    // Brute-forced seed
    // PS. Where is the 8, Nintendo?
    Random random(12345679);

    mVec3Tbl = static_cast<math::VEC3*>(AllocFromStaticHeap(sizeof(math::VEC3) * 0x200));
    mNormalizedVec3Tbl = static_cast<math::VEC3*>(AllocFromStaticHeap(sizeof(math::VEC3) * 0x200));

    for (s32 i = 0; i < 0x200; i++)
    {
        mVec3Tbl[i].x = random.GetF32Range(-1.0f, 1.0f);
        mVec3Tbl[i].y = random.GetF32Range(-1.0f, 1.0f);
        mVec3Tbl[i].z = random.GetF32Range(-1.0f, 1.0f);
        mNormalizedVec3Tbl[i].x = random.GetF32Range(-1.0f, 1.0f);
        mNormalizedVec3Tbl[i].y = random.GetF32Range(-1.0f, 1.0f);
        mNormalizedVec3Tbl[i].z = random.GetF32Range(-1.0f, 1.0f);
        mNormalizedVec3Tbl[i].Normalize();
    }
}

} } // namespace nw::eft
