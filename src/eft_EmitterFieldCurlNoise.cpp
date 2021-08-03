#include <eft_Emitter.h>
#include <eft_EmitterFieldCurlNoise.h>
#include <eft_Misc.h>

namespace nw { namespace eft {

#include "eft_EmitterFieldCurlNoise.hpp"

const void* EmitterCalc::_ptclField_CurlNoise(EmitterInstance* emitter, PtclInstance* ptcl, const void* fieldData, f32 emissionSpeed)
{
    const FieldCurlNoiseData* curlNoiseData = static_cast<const FieldCurlNoiseData*>(fieldData);

    f32 time = emitter->counter;

    f32 scale = curlNoiseData->scale;
    math::VEC3 weight = curlNoiseData->weight;
    math::VEC3 speed = curlNoiseData->speed;

    f32 offset = curlNoiseData->offset;
    if (curlNoiseData->randomOffset & 1)
        offset *= ptcl->randomVec4.x;

    u32 interpolation = curlNoiseData->interpolation & 1;

    f32 z = ptcl->pos.x * scale + speed.x * time + offset;
    f32 y = ptcl->pos.y * scale + speed.y * time + offset;
    f32 x = ptcl->pos.z * scale + speed.z * time + offset;

    s32 zS32 = (s32)z;
    s32 yS32 = (s32)y;
    s32 xS32 = (s32)x;

    CurlNoiseTbl& curlNoiseTbl = g_CurlNoiseTbl32;
    math::VEC3 velocity;

    if (interpolation == 0)
        velocity = curlNoiseTbl.GetVec3(zS32, yS32, xS32);

    else
    {
        f32 zDelta = z - zS32;
        f32 yDelta = y - yS32;
        f32 xDelta = x - xS32;
        f32 zDeltaInv = 1.0f - zDelta;
        f32 yDeltaInv = 1.0f - yDelta;
        f32 xDeltaInv = 1.0f - xDelta;

        math::VEC3 x0y0z0 = curlNoiseTbl.GetVec3(zS32 + 0, yS32 + 0, xS32 + 0);
        math::VEC3 x0y0z1 = curlNoiseTbl.GetVec3(zS32 + 1, yS32 + 0, xS32 + 0);
        math::VEC3 x0y1z0 = curlNoiseTbl.GetVec3(zS32 + 0, yS32 + 1, xS32 + 0);
        math::VEC3 x0y1z1 = curlNoiseTbl.GetVec3(zS32 + 1, yS32 + 1, xS32 + 0);
        math::VEC3 x1y0z0 = curlNoiseTbl.GetVec3(zS32 + 0, yS32 + 0, xS32 + 1);
        math::VEC3 x1y0z1 = curlNoiseTbl.GetVec3(zS32 + 1, yS32 + 0, xS32 + 1);
        math::VEC3 x1y1z0 = curlNoiseTbl.GetVec3(zS32 + 0, yS32 + 1, xS32 + 1);
        math::VEC3 x1y1z1 = curlNoiseTbl.GetVec3(zS32 + 1, yS32 + 1, xS32 + 1);

        math::VEC3 x0y0z, x0y1z, x0yz;
        math::VEC3 x1y0z, x1y1z, x1yz;

        math::VEC3::Scale(&x0y0z0, &x0y0z0, zDeltaInv);
        math::VEC3::Scale(&x0y0z1, &x0y0z1, zDelta);
        math::VEC3::Add(&x0y0z, &x0y0z0, &x0y0z1);

        math::VEC3::Scale(&x0y1z0, &x0y1z0, zDeltaInv);
        math::VEC3::Scale(&x0y1z1, &x0y1z1, zDelta);
        math::VEC3::Add(&x0y1z, &x0y1z0, &x0y1z1);

        math::VEC3::Scale(&x0y0z, &x0y0z, yDeltaInv);
        math::VEC3::Scale(&x0y1z, &x0y1z, yDelta);
        math::VEC3::Add(&x0yz, &x0y0z, &x0y1z);

        math::VEC3::Scale(&x1y0z0, &x1y0z0, zDeltaInv);
        math::VEC3::Scale(&x1y0z1, &x1y0z1, zDelta);
        math::VEC3::Add(&x1y0z, &x1y0z0, &x1y0z1);

        math::VEC3::Scale(&x1y1z0, &x1y1z0, zDeltaInv);
        math::VEC3::Scale(&x1y1z1, &x1y1z1, zDelta);
        math::VEC3::Add(&x1y1z, &x1y1z0, &x1y1z1);

        math::VEC3::Scale(&x1y0z, &x1y0z, yDeltaInv);
        math::VEC3::Scale(&x1y1z, &x1y1z, yDelta);
        math::VEC3::Add(&x1yz, &x1y0z, &x1y1z);

        math::VEC3::Scale(&x0yz, &x0yz, xDeltaInv);
        math::VEC3::Scale(&x1yz, &x1yz, xDelta);
        math::VEC3::Add(&velocity, &x0yz, &x1yz);
    }

    ptcl->velocity.x += velocity.x * weight.x;
    ptcl->velocity.y += velocity.y * weight.y;
    ptcl->velocity.z += velocity.z * weight.z;

    return curlNoiseData + 1;
}

void CurlNoiseTexture::Initialize()
{
    imagePtr = NULL;

    GX2SurfaceFormat format = GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM;
    GX2InitTexture(&gx2Texture, 32, 32, 32, 0, format, GX2_SURFACE_DIM_2D_ARRAY);
    gx2Texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;

    GX2CalcSurfaceSizeAndAlignment(&gx2Texture.surface);
    GX2InitTextureRegs(&gx2Texture);

    imagePtr = AllocFromStaticHeap(gx2Texture.surface.imageSize, gx2Texture.surface.alignment);
    GX2InitTexturePtrs(&gx2Texture, imagePtr, NULL);

    const u8* dataU8 = static_cast<const u8*>(&g_curlNoiseTbl32[0][0]);

    for (u32 z = 0; z < 32; z++)
        for (u32 y = 0; y < 32; y++)
            for (u32 x = 0; x < 32; x++)
                ((u32*)imagePtr)[z * (gx2Texture.surface.pitch * 32) + y * gx2Texture.surface.pitch + x] = (  *dataU8++ << 8
                                                                                                            | *dataU8++ << 16
                                                                                                            | *dataU8++ << 24
                                                                                                            | 0xFF  );

    DCFlushRange(gx2Texture.surface.imagePtr, gx2Texture.surface.imageSize);
}

void InitializeCurlNoise()
{
    g_CurlNoiseTbl32.length = 32;
    g_CurlNoiseTbl32.data = g_curlNoiseTbl32;
    g_CurlNoiseTexture.Initialize();
}

const GX2Texture* GetCurlNoiseTexture()
{
    return &g_CurlNoiseTexture.gx2Texture;
}

} }
