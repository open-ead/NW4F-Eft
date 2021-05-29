#include <eft_Heap.h>
#include <eft_ResData.h>
#include <eft_Resource.h>
#include <eft_Shader.h>

#include <new>

namespace nw { namespace eft {

Resource::Resource(Heap* heap, void* resource, u32 resourceID, System* system)
{
    Initialize(heap, resource, resourceID, system);
}

void Resource::Initialize(Heap* argHeap, void* argResource, u32 argResourceID, System* argSystem)
{
    system = argSystem;
    heap = argHeap;
    shaders = NULL;
    numShader = 0;
    primitives = NULL;
    numPrimitive = 0;

    resource = reinterpret_cast<Header*>(argResource);
    strTbl = reinterpret_cast<char*>((u32)argResource + resource->strTblOffs);
    textureDataTbl = reinterpret_cast<void*>((u32)argResource + resource->textureDataTblOffs);

    resourceID = argResourceID;

    if (resource->numEmitterSet != 0)
    {
        ShaderTable* shaderTbl = reinterpret_cast<ShaderTable*>((u32)argResource + resource->shaderTblOffs);
        ShaderProgram* program = reinterpret_cast<ShaderProgram*>((u32)shaderTbl + shaderTbl->shaderProgOffs);
        ShaderProgram* const baseProgram = program;

        numShader = shaderTbl->numShaderProg;
        shaders = static_cast<ParticleShader**>(heap->Alloc(sizeof(ParticleShader*) * numShader));

        for (u32 i = 0; i < numShader; i++)
        {
            shaders[i] = new (heap->Alloc(sizeof(ParticleShader))) ParticleShader();
            shaders[i]->vertexShaderKey = program[i].vertexShaderKey;
            shaders[i]->fragmentShaderKey = program[i].fragmentShaderKey;
            shaders[i]->geometryShaderKey = program[i].geometryShaderKey;
            shaders[i]->SetupShaderResource(heap, reinterpret_cast<void*>((u32)(baseProgram + shaderTbl->numShaderProg) + program[i].binOffs), program[i].binSize);
        }

        // ...
    }
}

} } // namespace nw::eft
