#ifndef EFT_RESOURCE_H_
#define EFT_RESOURCE_H_

#include <types.h>

namespace nw { namespace eft {

class Heap;
class ParticleShader;
class Primitive;
class System;

class Resource
{
public:
    Resource(Heap* heap, void* resource, u32 resourceID, System* system);
    virtual ~Resource();

    void Initialize(Heap* heap, void* resource, u32 resourceID, System* system);
    void Finalize(Heap* heap);

    System* system;
    u32 resourceID;
    void* resource;
    char* strTbl;
    void* textureDataTbl;
    void* emitterSets;
    Heap* heap;
    ParticleShader** shaders;
    u32 numShader;
    Primitive** primitives;
    u32 numPrimitive;
};

} } // namespace nw::eft

#endif // EFT_RESOURCE_H_
