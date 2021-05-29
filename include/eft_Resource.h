#ifndef EFT_RESOURCE_H_
#define EFT_RESOURCE_H_

#include <types.h>

namespace nw { namespace eft {

struct EmitterReference;
struct EmitterSetData;
struct Header;
class Heap;
class ParticleShader;
class Primitive;
class System;
struct TextureRes;

class Resource
{
public:
    struct EmitterSet // Actual name not known
    {
        EmitterSetData* data;
        EmitterReference* emitterRef;
        u32 numEmitter;
        u32 userData;

        // No idea why there are repeated
        EmitterSetData* _data;
        EmitterReference* _emitterRef;
        u32 _numEmitter;
        u32 _userData;

        const char* name;
        u8 _24;
        ParticleShader** shaders;
        u32 numShader;
        Primitive** primitives;
        u32 numPrimitive;
    };

public:
    Resource(Heap* heap, void* resource, u32 resourceID, System* system);
    virtual ~Resource();

    void CreateFtexbTextureHandle(Heap* heap, void* data, TextureRes& texture);
    void CreateOriginalTextureHandle(Heap* heap, void* data, TextureRes& texture);
    void Initialize(Heap* heap, void* resource, u32 resourceID, System* system);
    void Finalize(Heap* heap);

    System* system;
    u32 resourceID;
    Header* resource;
    char* strTbl;
    u8* textureDataTbl;
    EmitterSet* emitterSets;
    Heap* heap;
    ParticleShader** shaders;
    u32 numShader;
    Primitive** primitives;
    u32 numPrimitive;
};

} } // namespace nw::eft

#endif // EFT_RESOURCE_H_
