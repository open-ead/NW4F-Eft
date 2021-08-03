#include <math/math_VEC4.h>
#include <eft_Emitter.h>
#include <eft_Heap.h>
#include <eft_Misc.h>
#include <eft_Primitive.h>
#include <eft_ResData.h>
#include <eft_Resource.h>
#include <eft_Shader.h>

#include <new>

namespace nw { namespace eft {

Resource::Resource(Heap* heap, void* resource, u32 resourceID, System* system, bool _unusedArg)
{
    Initialize(heap, resource, resourceID, system, _unusedArg);
}

Resource::~Resource()
{
}

void Resource::CreateFtexbTextureHandle(Heap* heap, void* data, TextureRes& texture)
{
    GX2SurfaceFormat formats[] = {
        GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM,
        GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM,
        GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM,
        GX2_SURFACE_FORMAT_T_BC1_UNORM,
        GX2_SURFACE_FORMAT_T_BC1_SRGB,
        GX2_SURFACE_FORMAT_T_BC2_UNORM,
        GX2_SURFACE_FORMAT_T_BC2_SRGB,
        GX2_SURFACE_FORMAT_T_BC3_UNORM,
        GX2_SURFACE_FORMAT_T_BC3_SRGB,
        GX2_SURFACE_FORMAT_T_BC4_UNORM,
        GX2_SURFACE_FORMAT_T_BC4_SNORM,
        GX2_SURFACE_FORMAT_T_BC5_UNORM,
        GX2_SURFACE_FORMAT_T_BC5_SNORM,
        GX2_SURFACE_FORMAT_TC_R8_UNORM,
        GX2_SURFACE_FORMAT_TC_R8_G8_UNORM,
        GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_SRGB,
        GX2_SURFACE_FORMAT_TC_R8_SNORM,
        GX2_SURFACE_FORMAT_T_R4_G4_UNORM,
        GX2_SURFACE_FORMAT_TC_R11_G11_B10_FLOAT,
        GX2_SURFACE_FORMAT_TC_R16_FLOAT,
        GX2_SURFACE_FORMAT_TC_R16_G16_FLOAT,
        GX2_SURFACE_FORMAT_TC_R16_G16_B16_A16_FLOAT,
        GX2_SURFACE_FORMAT_TCD_R32_FLOAT,
        GX2_SURFACE_FORMAT_TC_R32_G32_FLOAT,
        GX2_SURFACE_FORMAT_TC_R32_G32_B32_A32_FLOAT,
        GX2_SURFACE_FORMAT_TCS_R5_G6_B5_UNORM,
        GX2_SURFACE_FORMAT_TC_R5_G5_B5_A1_UNORM,
    };

    GX2SurfaceFormat format = GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM;
    if (TextureResFormat_Invalid < texture.cafeTexFormat && texture.cafeTexFormat < TextureResFormat_Max)
        format = formats[texture.cafeTexFormat];
    else
        WARNING("Input FTX Texture Format is out of support.\n");

    if (texture.depth == 1)
        GX2InitTexture(&texture.gx2Texture, texture.width, texture.height, texture.depth, texture.numMips, format, GX2_SURFACE_DIM_2D);
    else
        GX2InitTexture(&texture.gx2Texture, texture.width, texture.height, texture.depth, texture.numMips, format, GX2_SURFACE_DIM_2D_ARRAY);
    texture.gx2Texture.surface.tileMode = texture.tileMode;

    GX2CalcSurfaceSizeAndAlignment(&texture.gx2Texture.surface);
    GX2SetSurfaceSwizzle(&texture.gx2Texture.surface, texture.swizzle);
    GX2InitTexturePtrs(&texture.gx2Texture, data, NULL);
    GX2InitTextureCompSel(&texture.gx2Texture, texture.compSel);
    GX2InitTextureRegs(&texture.gx2Texture);
    DCFlushRange(texture.gx2Texture.surface.imagePtr, texture.gx2Texture.surface.imageSize + texture.gx2Texture.surface.mipSize);

    texture.initialized = 1;
}

void Resource::CreateOriginalTextureHandle(Heap* heap, void* data, TextureRes& texture)
{
    GX2SurfaceFormat format = GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM;
    GX2InitTexture(&texture.gx2Texture, texture.width, texture.height, 1, 0, format, GX2_SURFACE_DIM_2D);
    texture.gx2Texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;

    GX2CalcSurfaceSizeAndAlignment(&texture.gx2Texture.surface);
    GX2InitTextureRegs(&texture.gx2Texture);

    void* data_aligned = heap->Alloc(texture.gx2Texture.surface.imageSize, texture.gx2Texture.surface.alignment);
    GX2InitTexturePtrs(&texture.gx2Texture, data_aligned, NULL);

    const u8* dataU8 = static_cast<const u8*>(data);

    if (texture.originalTexFormat == TextureResFormat_RGBA8_Unorm)
    {
        for (u32 y = 0; y < texture.height; y++)
            for (u32 x = 0; x < texture.width; x++)
                ((u32*)data_aligned)[y * texture.gx2Texture.surface.pitch + x] = (  *dataU8++ << 8
                                                                                  | *dataU8++ << 16
                                                                                  | *dataU8++ << 24
                                                                                  | *dataU8++  );
    }
    else
    {
        for (u32 y = 0; y < texture.height; y++)
            for (u32 x = 0; x < texture.width; x++)
                ((u32*)data_aligned)[y * texture.gx2Texture.surface.pitch + x] = (  *dataU8++ << 8
                                                                                  | *dataU8++ << 16
                                                                                  | *dataU8++ << 24
                                                                                  | 0xFF  );
    }

    DCFlushRange(texture.gx2Texture.surface.imagePtr, texture.gx2Texture.surface.imageSize);

    texture.initialized = 1;
}

void Resource::Initialize(Heap* argHeap, void* argResource, u32 argResourceID, System* argSystem, bool _unusedArg)
{
    system = argSystem;
    heap = argHeap;
    shaders = NULL;
    numShader = 0;
    primitives = NULL;
    numPrimitive = 0;

    resource = reinterpret_cast<Header*>(argResource);
    strTbl = reinterpret_cast<char*>((u32)argResource + resource->strTblOffs);
    textureDataTbl = reinterpret_cast<u8*>((u32)argResource + resource->textureDataTblOffs);

    const u32 magic = resource->magic[0] << 24 | resource->magic[1] << 16 | resource->magic[2] << 8 | resource->magic[3];
    if (magic != 0x45465446) // EFTF
        ERROR("Binary Target is Windows.\n");

    if (resource->version != 65)
        ERROR("Binary Version Error. Data Version:%d, Runtime Version:%d, DataName:%s.\n", resource->version, 65, strTbl);

    if (resource->numEmitterSet == 0)
    {
        WARNING("EmitterSet is Empty.\n");
        return;
    }

    resourceID = argResourceID;

    ShaderTable* shaderTbl = reinterpret_cast<ShaderTable*>((u32)argResource + resource->shaderTblOffs);
    ShaderProgram* program = reinterpret_cast<ShaderProgram*>((u32)shaderTbl + shaderTbl->shaderProgOffs);
    u8* const baseShaderBinary = reinterpret_cast<u8*>((u32)shaderTbl + shaderTbl->shaderBinOffs);

    numShader = shaderTbl->numShaderProg;
    //if (numShader != 0)
    {
        LOG("Resource Setup. Setup Shader Num : %d \n", numShader);
        shaders = static_cast<ParticleShader**>(heap->Alloc(sizeof(ParticleShader*) * numShader));

        for (u32 i = 0; i < numShader; i++)
        {
            shaders[i] = new (heap->Alloc(sizeof(ParticleShader))) ParticleShader();
            shaders[i]->vertexShaderKey = program[i].vertexShaderKey;
            shaders[i]->fragmentShaderKey = program[i].fragmentShaderKey;
            shaders[i]->geometryShaderKey = program[i].geometryShaderKey;
            shaders[i]->SetupShaderResource(heap, baseShaderBinary + program[i].binOffs, program[i].binSize);
        }
    }

    PrimitiveTable* primitiveTbl = reinterpret_cast<PrimitiveTable*>((u32)argResource + resource->primitiveTblOffs);
    PrimitiveData* primitiveData = reinterpret_cast<PrimitiveData*>((u32)primitiveTbl + primitiveTbl->primitiveOffs);
    u8* const baseAttribBuff = reinterpret_cast<u8*>(primitiveData + primitiveTbl->numPrimitive);

    numPrimitive = primitiveTbl->numPrimitive;
    if (numPrimitive != 0)
    {
        LOG("Resource Setup. Setup Primitive Num : %d \n", numPrimitive);
        primitives = static_cast<Primitive**>(heap->Alloc(sizeof(Primitive*) * numPrimitive));

        for (u32 i = 0; i < numPrimitive; i++)
        {
            primitives[i] = new (heap->Alloc(sizeof(Primitive))) Primitive();

            f32* const pos = reinterpret_cast<f32*>(baseAttribBuff + primitiveData[i].pos.bufferOffs);
            f32* const texCoord = reinterpret_cast<f32*>(baseAttribBuff + primitiveData[i].texCoord.bufferOffs);
            u32* const index = reinterpret_cast<u32*>(baseAttribBuff + primitiveData[i].index.bufferOffs);

            f32* const normal = (primitiveData[i].normal.bufferOffs != 0) ? reinterpret_cast<f32*>(baseAttribBuff + primitiveData[i].normal.bufferOffs) : NULL;
            f32* const color = (primitiveData[i].color.bufferOffs != 0) ? reinterpret_cast<f32*>(baseAttribBuff + primitiveData[i].color.bufferOffs) : NULL;

            const u32 posBufSize = primitiveData[i].pos.bufferSize;
            const u32 normalBufSize = primitiveData[i].normal.bufferSize;
            const u32 colorBufSize = primitiveData[i].color.bufferSize;
            const u32 texCoordBufSize = primitiveData[i].texCoord.bufferSize;
            const u32 indexBufSize = primitiveData[i].index.bufferSize;

            if (pos != NULL && texCoord != NULL && index != NULL)
            {
                Primitive* primitive = primitives[i];
                primitive->initialized = true;
                primitive->numIndex = primitiveData[i].index.count;

                if (pos != NULL)
                {
                    void* posBuf = primitive->vbPos.AllocateVertexBuffer(argHeap, posBufSize, 3);
                    memcpy(posBuf, pos, posBufSize);
                    primitive->pos = reinterpret_cast<f32*>(posBuf);
                    primitive->vbPos.Invalidate();
                }

                if (normal != NULL)
                {
                    void* normalBuf = primitive->vbNormal.AllocateVertexBuffer(argHeap, normalBufSize, 3);
                    memcpy(normalBuf, normal, normalBufSize);
                    primitive->normal = reinterpret_cast<f32*>(normalBuf);
                    primitive->vbNormal.Invalidate();
                }

                if (color != NULL)
                {
                    void* colorBuf = primitive->vbColor.AllocateVertexBuffer(argHeap, colorBufSize, 4);
                    memcpy(colorBuf, color, colorBufSize);
                    primitive->color = reinterpret_cast<f32*>(colorBuf);
                    primitive->vbColor.Invalidate();
                }
                else
                {
                    void* colorBuf = primitive->vbColor.AllocateVertexBuffer(argHeap, sizeof(math::VEC4) * primitive->numIndex, 4);
                    for (u32 j = 0; j < primitive->numIndex; j++)
                        ((math::VEC4*)colorBuf)[j] = (math::VEC4){ 1.0f, 1.0f, 1.0f, 1.0f };
                    primitive->color = reinterpret_cast<f32*>(colorBuf);
                    primitive->vbColor.Invalidate();
                }

                if (texCoord != NULL && texCoordBufSize != 0)
                {
                    void* texCoordBuf = primitive->vbTexCoord.AllocateVertexBuffer(argHeap, texCoordBufSize, 4);
                    memcpy(texCoordBuf, texCoord, texCoordBufSize);
                    primitive->texCoord = reinterpret_cast<f32*>(texCoordBuf);
                    primitive->vbTexCoord.Invalidate();
                }

                if (index != NULL && indexBufSize != 0)
                {
                    void* indexBuf = primitive->vbIndex.AllocateVertexBuffer(argHeap, indexBufSize, 1);
                    memcpy(indexBuf, index, indexBufSize);
                    primitive->index = reinterpret_cast<u32*>(indexBuf);
                    primitive->vbIndex.Invalidate();
                }
                else
                {
                    ERROR("Primitive Index is Error.\n");
                }
            }
        }
    }

    EmitterSetData* emitterSetData = reinterpret_cast<EmitterSetData*>(resource + 1);

    const s32 numEmitterSet = resource->numEmitterSet;
    //if (numEmitterSet != 0)
    {
        emitterSets = static_cast<Resource::EmitterSet*>(heap->Alloc(sizeof(Resource::EmitterSet) * numEmitterSet));

        u32 numEmitter = 0;
        for (s32 i = 0; i < numEmitterSet; i++)
        {
            Resource::EmitterSet* emitterSet = &emitterSets[i];
            emitterSet->data = &emitterSetData[i];
            numEmitter += emitterSet->data->numEmitter;
        }

        emitterStaticUniformBlocks = static_cast<EmitterStaticUniformBlock*>(heap->Alloc(sizeof(EmitterStaticUniformBlock) * numEmitter * 2, 0x100));

        u32 currentStaticUBIdx = 0;
        for (s32 i = 0; i < numEmitterSet; i++)
        {
            Resource::EmitterSet* emitterSet = &emitterSets[i];

            emitterSet->name = (emitterSet->data->name = &strTbl[emitterSet->data->nameOffs]);
            emitterSet->numEmitter = emitterSet->data->numEmitter;
            emitterSet->userData = emitterSet->data->userData;
            emitterSet->shaders = shaders;
            emitterSet->numShader = numShader;
            emitterSet->primitives = primitives;
            emitterSet->numPrimitive = numPrimitive;

            if (emitterSet->data->emitterRefOffs != 0)
            {
                emitterSet->emitterRef = (/*emitterSet->data->emitterRef =*/ reinterpret_cast<EmitterTblData*>((u32)argResource + emitterSet->data->emitterRefOffs));

                for (u32 j = 0; j < emitterSet->numEmitter; j++)
                {
                    EmitterTblData* emitterRef = &emitterSet->emitterRef[j];

                    if (emitterRef->dataOffs == 0)
                    {
                        emitterRef->data = NULL;
                        emitterRef->emitterStaticUniformBlock      = NULL;
                        emitterRef->childEmitterStaticUniformBlock = NULL;
                    }
                    else
                    {
                        emitterRef->data = reinterpret_cast<EmitterData*>((u32)argResource + emitterRef->dataOffs);
                        emitterRef->data->name = &strTbl[emitterRef->data->nameOffs];
                        emitterRef->emitterStaticUniformBlock = &emitterStaticUniformBlocks[currentStaticUBIdx++];
                        emitterRef->childEmitterStaticUniformBlock = &emitterStaticUniformBlocks[currentStaticUBIdx++];

                        const SimpleEmitterData* data = static_cast<const SimpleEmitterData*>(emitterRef->data);
                        const ComplexEmitterData* cdata = NULL;
                        if (emitterRef->data->type == EmitterType_Complex)
                            cdata = static_cast<const ComplexEmitterData*>(emitterRef->data);

                        EmitterInstance::UpdateEmitterStaticUniformBlock(emitterRef->emitterStaticUniformBlock, data, cdata);

                        for (s32 k = 0; k < 3; k++)
                        {
                            TextureRes* const texture = &emitterRef->data->textures[k];

                            if (texture->cafeTexDataSize > 0)
                                CreateFtexbTextureHandle(heap, textureDataTbl + texture->cafeTexDataOffs, *texture);

                            else if (texture->originalTexDataSize > 0)
                                CreateOriginalTextureHandle(heap, textureDataTbl + texture->originalTexDataOffs, *texture);

                            else if (k == 0)
                                ERROR("Texture Binary is None.\n");
                        }

                        if (emitterRef->data->type == EmitterType_Complex
                            && (static_cast<ComplexEmitterData*>(emitterRef->data)->childFlags & 1))
                        {
                            TextureRes* const texture = &reinterpret_cast<ChildData*>(static_cast<ComplexEmitterData*>(emitterRef->data) + 1)->texture;

                            if (texture->cafeTexDataSize > 0)
                                CreateFtexbTextureHandle(heap, textureDataTbl + texture->cafeTexDataOffs, *texture);

                            else if (texture->originalTexDataSize > 0)
                                CreateOriginalTextureHandle(heap, textureDataTbl + texture->originalTexDataOffs, *texture);

                            else
                                ERROR("Child Texture Binary is None.\n");
                        }

                        if (emitterRef->data->keyAnimArray.size != 0)
                            emitterRef->data->keyAnimArray.ptr = reinterpret_cast<KeyFrameAnimArray*>((u32)argResource + resource->keyAnimArrayTblOffs + emitterRef->data->keyAnimArray.offset);

                        if (emitterRef->data->shaderParam.count != 0)
                            emitterRef->data->shaderParam.ptr = reinterpret_cast<f32*>((u32)argResource + resource->shaderParamTblOffs + emitterRef->data->shaderParam.offset);

                        if (emitterRef->data->type == EmitterType_Complex
                            && (static_cast<ComplexEmitterData*>(emitterRef->data)->childFlags & 1))
                        {
                            ChildData* childData = reinterpret_cast<ChildData*>(static_cast<ComplexEmitterData*>(emitterRef->data) + 1);
                            if (childData->shaderParam.count != 0)
                                childData->shaderParam.ptr = reinterpret_cast<f32*>((u32)argResource + resource->shaderParamTblOffs + childData->shaderParam.offset);
                        }
                    }
                }
            }
            else
            {
                emitterSet->emitterRef = (/*emitterSet->data->emitterRef =*/ NULL);
            }

            emitterSet->_data = emitterSet->data;
            emitterSet->_emitterRef = emitterSet->emitterRef;
            emitterSet->_numEmitter = emitterSet->numEmitter;
            emitterSet->_userData = emitterSet->userData;
        }
    }
}

void Resource::DeleteTextureHandle(Heap* heap, TextureRes& texture, bool originalTexture)
{
    if (heap != NULL && originalTexture)
        heap->Free(texture.gx2Texture.surface.imagePtr);

    texture.initialized = 0;
}

void Resource::Finalize(Heap* heap)
{
    if (heap == NULL)
        heap = this->heap;

    for (s32 i = 0; i < resource->numEmitterSet; i++)
    {
        Resource::EmitterSet* emitterSet = &emitterSets[i];
        for (s32 j = 0; j < emitterSet->data->numEmitter; j++)
        {
            EmitterTblData* emitterRef = &emitterSet->emitterRef[j];

            for (s32 k = 0; k < 3; k++)
            {
                TextureRes* const texture = &emitterRef->data->textures[k];
                if (texture->initialized)
                    DeleteTextureHandle(heap, *texture, !texture->cafeTexDataSize);
            }

            if (emitterRef->data->type == EmitterType_Complex
                && (static_cast<ComplexEmitterData*>(emitterRef->data)->childFlags & 1))
            {
                TextureRes* const texture = &reinterpret_cast<ChildData*>(static_cast<ComplexEmitterData*>(emitterRef->data) + 1)->texture;
                if (texture->initialized)
                    DeleteTextureHandle(heap, *texture, !texture->cafeTexDataSize);
            }
        }
    }

    for (u32 i = 0; i < numShader; i++)
    {
        shaders[i]->Finalize(heap);
        heap->Free(shaders[i]);
    }

    heap->Free(shaders);

    if (emitterSets != NULL)
    {
        heap->Free(emitterSets);
        emitterSets = NULL;
    }

    if (primitives != NULL)
    {
        for (u32 i = 0; i < numPrimitive; i++)
        {
            primitives[i]->Finalize(heap);
            heap->Free(primitives[i]);
        }

        heap->Free(primitives);
    }

    if (emitterStaticUniformBlocks != NULL)
    {
        heap->Free(emitterStaticUniformBlocks);
        emitterStaticUniformBlocks = NULL;
    }
}

ParticleShader* Resource::GetShader(s32 emitterSetID, u32 index)
{
    u32 numShader = emitterSets[emitterSetID].numShader;
    if (numShader <= index)
        return NULL;

    ParticleShader** shaders = emitterSets[emitterSetID].shaders;

    return shaders[index];
}

} } // namespace nw::eft
