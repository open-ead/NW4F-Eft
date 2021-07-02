#include <math/math_VEC4.h>
#include <eft_Heap.h>
#include <eft_Primitive.h>
#include <eft_ResData.h>
#include <eft_Resource.h>
#include <eft_Shader.h>

#include <new>

namespace nw { namespace eft {

Resource::Resource(Heap* heap, void* resource, u32 resourceID, System* system)
{
    Initialize(heap, resource, resourceID, system);
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
    };

    GX2SurfaceFormat format = GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM;
    if (TextureResFormat_Invalid < texture.cafeTexFormat && texture.cafeTexFormat < TextureResFormat_Max)
        format = formats[texture.cafeTexFormat];

    GX2InitTexture(&texture.gx2Texture, texture.width, texture.height, 1, texture.numMips, format, GX2_SURFACE_DIM_2D);
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
                ((u32*)data_aligned)[y * texture.gx2Texture.surface.pitch + x] = (  *dataU8++ << 24
                                                                                  | *dataU8++ << 16
                                                                                  | *dataU8++ << 8
                                                                                  | *dataU8++  );
    }
    else
    {
        for (u32 y = 0; y < texture.height; y++)
            for (u32 x = 0; x < texture.width; x++)
                ((u32*)data_aligned)[y * texture.gx2Texture.surface.pitch + x] = (  *dataU8++ << 24
                                                                                  | *dataU8++ << 16
                                                                                  | *dataU8++ << 8
                                                                                  | 0xFF  );
    }

    DCFlushRange(texture.gx2Texture.surface.imagePtr, texture.gx2Texture.surface.imageSize);

    texture.initialized = 1;
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
    textureDataTbl = reinterpret_cast<u8*>((u32)argResource + resource->textureDataTblOffs);

    resourceID = argResourceID;

    if (resource->numEmitterSet == 0)
        return;

    ShaderTable* shaderTbl = reinterpret_cast<ShaderTable*>((u32)argResource + resource->shaderTblOffs);
    ShaderProgram* program = reinterpret_cast<ShaderProgram*>((u32)shaderTbl + shaderTbl->shaderProgOffs);
    u8* const baseShaderBinary = reinterpret_cast<u8*>(program + shaderTbl->numShaderProg);

    numShader = shaderTbl->numShaderProg;
    //if (numShader != 0)
    {
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
                    //primitive->color = reinterpret_cast<f32*>(colorBuf); <-- NSMBU doesn't do this, but MK8 does. Bug in older Eft?
                    primitive->vbColor.Invalidate();
                }

                if (texCoord != NULL && texCoordBufSize != 0)
                {
                    void* texCoordBuf = primitive->vbTexCoord.AllocateVertexBuffer(argHeap, texCoordBufSize, 2);
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
            }
        }
    }

    EmitterSetData* emitterSetData = reinterpret_cast<EmitterSetData*>(resource + 1);

    const u32 numEmitterSet = resource->numEmitterSet;
    //if (numEmitterSet != 0)
    {
        emitterSets = static_cast<Resource::EmitterSet*>(heap->Alloc(sizeof(Resource::EmitterSet) * numEmitterSet));

        for (u32 i = 0; i < numEmitterSet; i++)
        {
            Resource::EmitterSet* emitterSet = &emitterSets[i];
            emitterSet->data = &emitterSetData[i];
            emitterSet->name = (emitterSet->data->name = &strTbl[emitterSet->data->nameOffs]);
            emitterSet->numEmitter = emitterSet->data->numEmitter;
            emitterSet->userData = emitterSet->data->userData;
            emitterSet->_unused = 0;
            emitterSet->shaders = shaders;
            emitterSet->numShader = numShader;
            emitterSet->primitives = primitives;
            emitterSet->numPrimitive = numPrimitive;

            if(emitterSet->data->emitterRefOffs != 0)
            {
                emitterSet->emitterRef = (/*emitterSet->data->emitterRef =*/ reinterpret_cast<EmitterTblData*>((u32)argResource + emitterSet->data->emitterRefOffs));

                for (u32 j = 0; j < emitterSet->numEmitter; j++)
                {
                    EmitterTblData* emitterRef = &emitterSet->emitterRef[j];

                    if (emitterRef->dataOffs == 0)
                        emitterRef->data = NULL;

                    else
                    {
                        emitterRef->data = reinterpret_cast<EmitterData*>((u32)argResource + emitterRef->dataOffs);
                        emitterRef->data->name = &strTbl[emitterRef->data->nameOffs];

                        {
                            TextureRes* const texture = &emitterRef->data->textures[0];

                            if (texture->cafeTexDataSize > 0)
                                CreateFtexbTextureHandle(heap, textureDataTbl + texture->cafeTexDataOffs, *texture);

                            else if (texture->originalTexDataSize > 0)
                                CreateOriginalTextureHandle(heap, textureDataTbl + texture->originalTexDataOffs, *texture);
                        }

                        {
                            TextureRes* const texture = &emitterRef->data->textures[1];

                            if (texture->cafeTexDataSize > 0)
                                CreateFtexbTextureHandle(heap, textureDataTbl + texture->cafeTexDataOffs, *texture);

                            else if (texture->originalTexDataSize > 0)
                                CreateOriginalTextureHandle(heap, textureDataTbl + texture->originalTexDataOffs, *texture);
                        }

                        if (emitterRef->data->type == EmitterType_Complex
                            && (static_cast<ComplexEmitterData*>(emitterRef->data)->childFlags & 1))
                        {
                            TextureRes* const texture = &reinterpret_cast<ChildData*>(static_cast<ComplexEmitterData*>(emitterRef->data) + 1)->texture;

                            if (texture->cafeTexDataSize > 0)
                                CreateFtexbTextureHandle(heap, textureDataTbl + texture->cafeTexDataOffs, *texture);

                            else if (texture->originalTexDataSize > 0)
                                CreateOriginalTextureHandle(heap, textureDataTbl + texture->originalTexDataOffs, *texture);
                        }

                        if (emitterRef->data->keyAnimArraySize != 0)
                            emitterRef->data->keyAnimArray = reinterpret_cast<void*>((u32)argResource + resource->keyAnimArrayTblOffs + emitterRef->data->keyAnimArrayOffs);
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

            {
                TextureRes* const texture = &emitterRef->data->textures[0];
                if (texture->initialized)
                    DeleteTextureHandle(heap, *texture, !texture->cafeTexDataSize);
            }

            {
                TextureRes* const texture = &emitterRef->data->textures[1];
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
}

ParticleShader* Resource::GetShader(s32 emitterSetID, const VertexShaderKey* vertexShaderKey, const FragmentShaderKey* fragmentShaderKey)
{
    u32 numShader = emitterSets[emitterSetID].numShader;
    ParticleShader** shaders = emitterSets[emitterSetID].shaders;

    for (u32 i = 0; i < numShader; i++)
        if (shaders[i]->vertexShaderKey == *vertexShaderKey && shaders[i]->fragmentShaderKey == *fragmentShaderKey)
            return shaders[i];

    return NULL;
}

} } // namespace nw::eft
