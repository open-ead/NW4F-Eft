#include <eft_CafeWrapper.h>
#include <eft_Heap.h>

#include <cafe/gfd.h>

BOOL _DEMOGFDReadVertexShader(nw::eft::Heap* heap, GX2VertexShader** shader, u32 index, const void* binary)
{
    if (binary == NULL || shader == NULL || index >= GFDGetVertexShaderCount(binary))
        return FALSE;

    u32 headerSize = GFDGetVertexShaderHeaderSize(index, binary);
    u32 programSize = GFDGetVertexShaderProgramSize(index, binary);

    if (headerSize == 0 || programSize == 0)
        return FALSE;

    GX2VertexShader* pHeader = static_cast<GX2VertexShader*>(heap->Alloc(headerSize, PPC_IO_BUFFER_ALIGN));
    void* pProgram = heap->Alloc(programSize, GX2_SHADER_ALIGNMENT);

    BOOL success = GFDGetVertexShader(pHeader, pProgram, index, binary);
    if (success)
    {
        DCFlushRange(pHeader->shaderPtr, pHeader->shaderSize);
        *shader = pHeader;
    }
    else
    {
        OSReport("Warning: Invalid Vertex Shader :%d", 0);
        if(pHeader) heap->Free(pHeader);
        if(pProgram) heap->Free(pProgram);
    }

    return success;
}

BOOL _DEMOGFDReadPixelShader(nw::eft::Heap* heap, GX2PixelShader** shader, u32 index, const void* binary)
{
    if (binary == NULL || shader == NULL || index >= GFDGetPixelShaderCount(binary))
        return FALSE;

    u32 headerSize = GFDGetPixelShaderHeaderSize(index, binary);
    u32 programSize = GFDGetPixelShaderProgramSize(index, binary);

    if (headerSize == 0 || programSize == 0)
        return FALSE;

    GX2PixelShader* pHeader = static_cast<GX2PixelShader*>(heap->Alloc(headerSize, PPC_IO_BUFFER_ALIGN));
    void* pProgram = heap->Alloc(programSize, GX2_SHADER_ALIGNMENT);

    BOOL success = GFDGetPixelShader(pHeader, pProgram, index, binary);
    if (success)
    {
        DCFlushRange(pHeader->shaderPtr, pHeader->shaderSize);
        *shader = pHeader;
    }
    else
    {
        OSReport("Warning: Invalid Pixel Shader :%d", 0);
        if(pHeader) heap->Free(pHeader);
        if(pProgram) heap->Free(pProgram);
    }

    return success;
}

BOOL _DEMOGFDReadGeometryShader(nw::eft::Heap* heap, GX2GeometryShader** shader, u32 index, const void* binary)
{
        if (binary == NULL || shader == NULL || index >= GFDGetGeometryShaderCount(binary))
        return FALSE;

    u32 headerSize = GFDGetGeometryShaderHeaderSize(index, binary);
    u32 programSize = GFDGetGeometryShaderProgramSize(index, binary);
    u32 copyProgramSize = GFDGetGeometryShaderCopyProgramSize(index, binary);

    if (headerSize == 0 || programSize == 0)
        return FALSE;

    GX2GeometryShader* pHeader = static_cast<GX2GeometryShader*>(heap->Alloc(headerSize, PPC_IO_BUFFER_ALIGN));
    void* pProgram = heap->Alloc(programSize, GX2_SHADER_ALIGNMENT);
    void* pCopyProgram = heap->Alloc(copyProgramSize, GX2_SHADER_ALIGNMENT);

    BOOL success = GFDGetGeometryShader(pHeader, pProgram, pCopyProgram, index, binary);
    if (success)
    {
        DCFlushRange(pHeader->shaderPtr, pHeader->shaderSize);
        DCFlushRange(pHeader->copyShaderPtr, pHeader->copyShaderSize);
        *shader = pHeader;
    }
    else
    {
        OSReport("Warning: Invalid Geometry Shader :%d", 0);
        if(pHeader) heap->Free(pHeader);
        if(pProgram) heap->Free(pProgram);
        if(pCopyProgram) heap->Free(pCopyProgram);
    }

    return success;
}

namespace nw { namespace eft {

TextureSampler::TextureSampler()
{
    GX2InitSampler(&sampler, GX2_TEX_CLAMP_WRAP, GX2_TEX_XY_FILTER_BILINEAR);
}

bool TextureSampler::Setup(TextureFilterMode filterMode, TextureWrapMode wrapModeX, TextureWrapMode wrapModeY)
{
    if (filterMode == TextureFilterMode_Linear)
        GX2InitSamplerXYFilter(&sampler, GX2_TEX_XY_FILTER_BILINEAR, GX2_TEX_XY_FILTER_BILINEAR, GX2_TEX_ANISO_1_TO_1);

    else
        GX2InitSamplerXYFilter(&sampler, GX2_TEX_XY_FILTER_POINT, GX2_TEX_XY_FILTER_POINT, GX2_TEX_ANISO_1_TO_1);

    GX2TexClamp clampX = GX2_TEX_CLAMP_MIRROR;
    switch(wrapModeX)
    {
    case TextureWrapMode_Mirror:
        clampX = GX2_TEX_CLAMP_MIRROR;
        break;
    case TextureWrapMode_Wrap:
        clampX = GX2_TEX_CLAMP_WRAP;
        break;
    case TextureWrapMode_Clamp:
        clampX = GX2_TEX_CLAMP_CLAMP;
        break;
    case TextureWrapMode_Mirror_Once:
        clampX = GX2_TEX_CLAMP_MIRROR_ONCE;
        break;
    }

    GX2TexClamp clampY = GX2_TEX_CLAMP_MIRROR;
    switch(wrapModeY)
    {
    case TextureWrapMode_Mirror:
        clampY = GX2_TEX_CLAMP_MIRROR;
        break;
    case TextureWrapMode_Wrap:
        clampY = GX2_TEX_CLAMP_WRAP;
        break;
    case TextureWrapMode_Clamp:
        clampY = GX2_TEX_CLAMP_CLAMP;
        break;
    case TextureWrapMode_Mirror_Once:
        clampY = GX2_TEX_CLAMP_MIRROR_ONCE;
        break;
    }

    GX2InitSamplerClamping(&sampler, clampX, clampY, GX2_TEX_CLAMP_WRAP);
    return true;
}

VertexBuffer::VertexBuffer()
{
    bufferSize = 0;
    buffer = NULL;
}

void* VertexBuffer::AllocateVertexBuffer(Heap* heap, u32 bufSize, u32 elemSize)
{
    bufferSize = bufSize;
    buffer = heap->Alloc(bufferSize, GX2_VERTEX_BUFFER_ALIGNMENT);
    size = elemSize;
    return buffer;
}

void VertexBuffer::Finalize(Heap* heap)
{
    if (buffer != NULL)
    {
        heap->Free(buffer);
        buffer = NULL;
    }
}

void VertexBuffer::Invalidate()
{
    DCFlushRange(buffer, bufferSize);
}

Shader::Shader()
{
    vertexShader = NULL;
    pixelShader = NULL;
    geometryShader = NULL;
    fetchShaderBufPtr = NULL;
    numAttribute = 0;
    initialized = false;
}

void Shader::Finalize(Heap* heap)
{
    if (vertexShader != NULL && vertexShader->shaderPtr != NULL)
    {
        heap->Free(vertexShader->shaderPtr);
        heap->Free(vertexShader);
    }

    if (pixelShader != NULL && pixelShader->shaderPtr != NULL)
    {
        heap->Free(pixelShader->shaderPtr);
        heap->Free(pixelShader);
    }

    if (geometryShader != NULL && geometryShader->shaderPtr != NULL)
    {
        heap->Free(geometryShader->shaderPtr);
        heap->Free(geometryShader);
    }

    if (fetchShaderBufPtr != NULL)
    {
        heap->Free(fetchShaderBufPtr);
        fetchShaderBufPtr = NULL;
    }
}

void Shader::BindShader()
{
    if (geometryShader != NULL)
        GX2SetShadersEx(&fetchShader, vertexShader, geometryShader, pixelShader);
    else
        GX2SetShaders(&fetchShader, vertexShader, pixelShader);
}

bool Shader::CreateShader(Heap* heap, const void* binary, u32 binarySize)
{
    _DEMOGFDReadVertexShader(heap, &vertexShader, 0, binary);
    _DEMOGFDReadPixelShader(heap, &pixelShader, 0, binary);
    _DEMOGFDReadGeometryShader(heap, &geometryShader, 0, binary);

    return true;
}

s32 Shader::GetFragmentSamplerLocation(const char* name)
{
    return GX2GetPixelSamplerVarLocation(pixelShader, name);
}

s32 Shader::GetAttributeLocation(const char* name)
{
    return GX2GetVertexAttribVarLocation(vertexShader, name);
}

u32 Shader::GetAttribute(const char* name, u32 buffer, VertexFormat attribFormat, u32 offset, bool instanceID)
{
    s32 location = GetAttributeLocation(name);
    if (location == -1)
        return 0xFFFFFFFF;

    attributeBuffer[location] = buffer;
    GX2InitAttribStream(&attributes[numAttribute], location, buffer, offset * sizeof(u32), static_cast<GX2AttribFormat>(attribFormat));

    if (instanceID)
    {
        attributes[numAttribute].indexType = GX2_ATTRIB_INDEX_INSTANCE_ID;
        attributes[numAttribute].aluDivisor = 1;
    }

    numAttribute++;
    return buffer;
}

void Shader::SetupShader(Heap* heap)
{
    fetchShaderBufPtr = heap->Alloc(GX2CalcFetchShaderSize(numAttribute), GX2_SHADER_ALIGNMENT);
    GX2InitFetchShader(&fetchShader, fetchShaderBufPtr, numAttribute, attributes);
    DCFlushRange(fetchShaderBufPtr, GX2CalcFetchShaderSize(numAttribute));
    initialized = true;
}

bool UniformBlock::InitializeVertexUniformBlock(Shader* shader, const char* name, u32)
{
    GX2UniformBlock* uniformBlock = GX2GetVertexUniformBlock(shader->vertexShader, name);
    if (uniformBlock == NULL)
    {
        blockNotExist = true;
        return false;
    }

    location = uniformBlock->location;
    bufferSize = uniformBlock->size;
    shaderStage = 0;
    initialized = true;

    return true;
}

bool UniformBlock::InitializePixelUniformBlock(Shader* shader, const char* name, u32)
{
    GX2UniformBlock* uniformBlock = GX2GetPixelUniformBlock(shader->pixelShader, name);
    if (uniformBlock == NULL)
    {
        blockNotExist = true;
        return false;
    }

    location = uniformBlock->location;
    bufferSize = uniformBlock->size;
    shaderStage = 1;
    initialized = true;

    return true;
}

} } // namespace nw::eft
