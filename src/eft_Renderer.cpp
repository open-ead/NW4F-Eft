#include <eft_Config.h>
#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_Misc.h>
#include <eft_Particle.h>
#include <eft_Primitive.h>
#include <eft_Renderer.h>
#include <eft_Shader.h>
#include <eft_System.h>
#include <eft_UniformBlock.h>

#include <cstdlib>

namespace nw { namespace eft {

s32 Renderer::ComparePtclViewZ(const void* a, const void* b)
{
    const PtclViewZ* ptcl_a = static_cast<const PtclViewZ*>(a);
    const PtclViewZ* ptcl_b = static_cast<const PtclViewZ*>(b);

    if (ptcl_a->z < 0.0f && ptcl_b->z < 0.0f)
    {
        if (ptcl_a->z < ptcl_b->z)
            return -1;
    }
    else
    {
        if (ptcl_a->z > ptcl_b->z)
            return -1;
    }

    return 1;
}

Renderer::Renderer(System* argSystem, const Config& config)
    : system(argSystem)
{
    depthBufferTextureOffset.x = 0.0f;
    depthBufferTextureOffset.y = 0.0f;
    depthBufferTextureScale.x = 1.0f;
    depthBufferTextureScale.y = 1.0f;
    frameBufferTextureOffset.x = 0.0f;
    frameBufferTextureOffset.y = 0.0f;
    frameBufferTextureScale.x = 1.0f;
    frameBufferTextureScale.y = 1.0f;

    currentParticleType = PtclType_Max;
    shaderType = ShaderType_Normal;
    renderVisibilityFlags = 0x3F;
    numDrawEmitter = 0;
    numDrawParticle = 0;
    _unused = 1;

    primitive = NULL;
    primitiveMode = 0x13;

    for (u32 i = 0; i < TextureSlot_Max; i++)
        textures[i] = NULL;

    math::VEC3* pos = static_cast<nw::math::VEC3*>(AllocFromStaticHeap(sizeof(nw::math::VEC3) * 4));
    pos[0].x = -0.5f;
    pos[0].y = 0.5f;
    pos[0].z = 0.0f;
    pos[1].x = -0.5f;
    pos[1].y = -0.5f;
    pos[1].z = 0.0f;
    pos[2].x = 0.5f;
    pos[2].y = -0.5f;
    pos[2].z = 0.0f;
    pos[3].x = 0.5f;
    pos[3].y = 0.5f;
    pos[3].z = 0.0f;
    vbPos.SetVertexBuffer(pos, sizeof(nw::math::VEC3) * 4, 3);
    vbPos.Invalidate();

    u32* index = static_cast<u32*>(AllocFromStaticHeap(sizeof(u32) * 4));
    index[0] = 0;
    index[1] = 1;
    index[2] = 2;
    index[3] = 3;
    vbIndex.SetVertexBuffer(index, sizeof(u32) * 4, 1);
    vbIndex.Invalidate();

    u32 doubleBufferSize = config.doubleBufferSize;
    doubleBufferSize += config.numParticleMax * sizeof(PtclAttributeBuffer);
    doubleBufferSize += 64                    * sizeof(ViewUniformBlock);
    doubleBufferSize += config.numEmitterMax  * sizeof(EmitterDynamicUniformBlock*);

    doubleBuffer.Initialize(doubleBufferSize, config.isTripleBuffer);
}

void Renderer::BeginRender(const math::MTX44& proj, const math::MTX34& view, const math::VEC3& cameraWorldPos, f32 zNear, f32 zFar)
{
    renderContext.SetupCommonState();

    this->view = math::MTX44(view);

    math::MTX34 billboard34;
    math::MTX34::Transpose(&billboard34, &view);

    billboard.m[0][0] = billboard34.m[0][0];
    billboard.m[0][1] = billboard34.m[0][1];
    billboard.m[0][2] = billboard34.m[0][2];
    billboard.m[0][3] = 0.0f;
    billboard.m[1][0] = billboard34.m[1][0];
    billboard.m[1][1] = billboard34.m[1][1];
    billboard.m[1][2] = billboard34.m[1][2];
    billboard.m[1][3] = 0.0f;
    billboard.m[2][0] = billboard34.m[2][0];
    billboard.m[2][1] = billboard34.m[2][1];
    billboard.m[2][2] = billboard34.m[2][2];
    billboard.m[2][3] = 0.0f;
    billboard.m[3][0] = 0.0f;
    billboard.m[3][1] = 0.0f;
    billboard.m[3][2] = 0.0f;
    billboard.m[3][3] = 1.0f;

    eyeVec.x = billboard34.m[0][2];
    eyeVec.y = billboard34.m[1][2];
    eyeVec.z = billboard34.m[2][2];

    eyePos = cameraWorldPos;

    // Why? Couldn't you have used this->view?
    math::MTX44 view44;
    view44.m[0][0] = view.m[0][0];
    view44.m[0][1] = view.m[0][1];
    view44.m[0][2] = view.m[0][2];
    view44.m[0][3] = view.m[0][3];
    view44.m[1][0] = view.m[1][0];
    view44.m[1][1] = view.m[1][1];
    view44.m[1][2] = view.m[1][2];
    view44.m[1][3] = view.m[1][3];
    view44.m[2][0] = view.m[2][0];
    view44.m[2][1] = view.m[2][1];
    view44.m[2][2] = view.m[2][2];
    view44.m[2][3] = view.m[2][3];
    view44.m[3][0] = 0.0f;
    view44.m[3][1] = 0.0f;
    view44.m[3][2] = 0.0f;
    view44.m[3][3] = 1.0f;

    math::MTX44::Concat(&viewProj, &proj , &view44);

    viewUniformBlock = static_cast<ViewUniformBlock*>(AllocFromDoubleBuffer(sizeof(ViewUniformBlock)));
    if (viewUniformBlock != NULL)
    {
        viewUniformBlock->viewMat = this->view;
        viewUniformBlock->projMat = proj;
        viewUniformBlock->vpMat = viewProj;
        viewUniformBlock->bldMat = billboard;
        viewUniformBlock->eyeVec.xyz() = eyeVec;
        viewUniformBlock->eyeVec.w = 0.0f;
        viewUniformBlock->eyePos.xyz() = eyePos;
        viewUniformBlock->eyePos.w = 0.0f;
        viewUniformBlock->depthBufferTexMat.xy() = depthBufferTextureScale;
        viewUniformBlock->depthBufferTexMat.zw() = depthBufferTextureOffset;
        viewUniformBlock->frameBufferTexMat.xy() = frameBufferTextureScale;
        viewUniformBlock->frameBufferTexMat.zw() = frameBufferTextureOffset;
        viewUniformBlock->viewParam.x = zNear;
        viewUniformBlock->viewParam.y = zFar;
        viewUniformBlock->viewParam.z = zNear * zFar;
        viewUniformBlock->viewParam.w = zFar - zNear;

        GX2EndianSwap(viewUniformBlock, sizeof(ViewUniformBlock));
        GX2Invalidate(GX2_INVALIDATE_CPU_UNIFORM_BLOCK, viewUniformBlock, sizeof(ViewUniformBlock));

        _unused = 1;
    }
}

bool Renderer::SetupParticleShaderAndVertex(ParticleShader* shader, MeshType meshType, Primitive* primitive)
{
    shader->Bind();

    {
        const ViewUniformBlock* viewUniformBlock = this->viewUniformBlock;
        shader->vertexViewUniformBlock.BindUniformBlock(viewUniformBlock);
        shader->fragmentViewUniformBlock.BindUniformBlock(viewUniformBlock);
    }

    _unused = 0;

    if (meshType == MeshType_Particle)
    {
        vbPos.BindBuffer(shader->attrPosBuffer, sizeof(math::VEC3) * 4, sizeof(math::VEC3));
        if (shader->attrIndexBuffer != 0xFFFFFFFF)
            vbIndex.BindBuffer(shader->attrIndexBuffer, sizeof(u32) * 4, sizeof(u32));

        primitiveMode = 0x13;
        this->primitive = NULL;
    }
    else
    {
        if (primitive == NULL || primitive->numIndex == 0)
            return false;

        this->primitive = primitive;
        if (!primitive->initialized)
            return false;

        {
            if (shader->attrPosBuffer != 0xFFFFFFFF && primitive->vbPos.buffer != NULL)
                primitive->vbPos.BindBuffer(shader->attrPosBuffer, primitive->vbPos.bufferSize, sizeof(math::VEC3));
        }

        {
            if (shader->attrTexCoordBuffer != 0xFFFFFFFF && primitive->vbTexCoord.buffer != NULL)
                primitive->vbTexCoord.BindBuffer(shader->attrTexCoordBuffer, primitive->vbTexCoord.bufferSize, sizeof(math::VEC4));
        }

        {
            if (shader->attrColorBuffer != 0xFFFFFFFF && primitive->vbColor.buffer != NULL)
                primitive->vbColor.BindBuffer(shader->attrColorBuffer, primitive->vbColor.bufferSize, sizeof(ut::Color4f));
        }

        u32 attrNormalBuffer = shader->attrNormalBuffer;
        if (attrNormalBuffer != 0xFFFFFFFF)
        {
            if (attrNormalBuffer != 0xFFFFFFFF && primitive->vbNormal.buffer != NULL)
                primitive->vbNormal.BindBuffer(attrNormalBuffer, primitive->vbNormal.bufferSize, sizeof(math::VEC3));
        }

        primitiveMode = 4;
    }

    return true;
}

void Renderer::SetupTexture(ParticleShader* shader, const TextureRes* texture0, const TextureRes* texture1, const TextureRes* texture2)
{
    if (texture0 != NULL && texture0->initialized != 0)
        renderContext.SetupTexture(texture0, TextureSlot_0, shader->fragmentSamplerLocations[0]);

    if (texture1 != NULL && texture1->initialized != 0)
        renderContext.SetupTexture(texture1, TextureSlot_1, shader->fragmentSamplerLocations[1]);

    if (texture2 != NULL && texture2->initialized != 0)
        renderContext.SetupTexture(texture2, TextureSlot_2, shader->fragmentSamplerLocations[2]);

    for (u32 i = TextureSlot_Frame_Buffer; i < TextureSlot_Max; i++)
    {
        if (textures[i] != NULL && shader->fragmentSamplerLocations[i].location != 0xFFFFFFFF)
        {
            if (i == TextureSlot_Curl_Noise)
                renderContext.SetupUserFragment2DArrayTexture(textures[i], TextureSlot(i), shader->fragmentSamplerLocations[i]);

            else
                renderContext.SetupTexture(textures[i], TextureSlot(i), shader->fragmentSamplerLocations[i]);
        }

        if (textures[i] != NULL && shader->vertexSamplerLocations[i].location != 0xFFFFFFFF)
        {
            if (i == TextureSlot_Curl_Noise)
                renderContext.SetupVertexArrayTexture(textures[i], TextureSlot(i), shader->vertexSamplerLocations[i]);

            else
                renderContext.SetupUserVertexTexture(textures[i], TextureSlot(i), shader->vertexSamplerLocations[i]);
        }
    }
}

void Renderer::DrawCpuEntry(ParticleShader* shader, u32 primitiveMode, u32 firstVertex, u32 numInstances, PtclAttributeBuffer* ptclAttributeBuffer, Primitive* primitive)
{
    if (ptclAttributeBuffer != NULL)
        BindParticleAttributeBlock(ptclAttributeBuffer, shader, firstVertex, numInstances);

    if (primitive != NULL)
        GX2DrawIndexedEx(static_cast<GX2PrimitiveType>(primitiveMode), primitive->numIndex, GX2_INDEX_FORMAT_U32, primitive->vbIndex.buffer, 0, numInstances);

    else
        GX2DrawEx(static_cast<GX2PrimitiveType>(primitiveMode), 4, 0, numInstances);
}

void Renderer::DrawGpuEntry(ParticleShader* shader, u32 primitiveMode, u32 firstVertex, u32 numInstances, PtclAttributeBufferGpu* ptclAttributeBufferGpu, Primitive* primitive)
{
    if (ptclAttributeBufferGpu != NULL)
        BindGpuParticleAttributeBlock(ptclAttributeBufferGpu, shader, firstVertex, numInstances);

    if (primitive != NULL)
        GX2DrawIndexedEx(static_cast<GX2PrimitiveType>(primitiveMode), primitive->numIndex, GX2_INDEX_FORMAT_U32, primitive->vbIndex.buffer, 0, numInstances);

    else
        GX2DrawEx(static_cast<GX2PrimitiveType>(primitiveMode), 4, 0, numInstances);
}

void Renderer::RequestParticle(const EmitterInstance* emitter, ParticleShader* shader, bool isChild, void* argData, bool draw)
{
    const SimpleEmitterData* data = emitter->data;

    u32 numParticles = emitter->numParticles;
    CustomShaderCallBackID callbackID = static_cast<CustomShaderCallBackID>(data->shaderUserSetting);
    ZBufATestType zBufATestType = data->zBufATestType;
    BlendType blendType = data->blendType;
    DisplaySideType displaySideType = data->displaySideType;
    MeshType meshType = data->meshType;
    bool gpuCalc = shader->vertexShaderKey.flags[0] & 0x2000000;

    if (isChild)
    {
        const ChildData* childData = emitter->GetChildData();

        numParticles = emitter->numChildParticles;
        callbackID = static_cast<CustomShaderCallBackID>(childData->shaderUserSetting);
        zBufATestType = childData->zBufATestType;
        blendType = childData->blendType;
        displaySideType = childData->displaySideType;
        meshType = childData->meshType;
    }

    renderContext.SetupZBufATest(zBufATestType);
    renderContext.SetupBlendType(blendType);
    renderContext.SetupDisplaySideType(displaySideType);

    if (system->GetCustomShaderRenderStateSetCallback(callbackID) != NULL)
    {
        RenderStateSetArg arg = {
            .emitter = emitter,
            .renderer = this,
            .flushCache = true,
            .argData = argData,
        };
        system->GetCustomShaderRenderStateSetCallback(callbackID)(arg);
    }

    else if (callbackID != CustomShaderCallBackID_Invalid)
    {
        RenderStateSetArg arg = {
            .emitter = emitter,
            .renderer = this,
            .flushCache = true,
            .argData = argData,
        };
        shader->SetUserUniformBlockFromData(arg, (ParticleShader::UserUniformBlockID)9, "userUniformBlockParam");
    }

    if (system->GetDrawPathRenderStateSetCallback(static_cast<DrawPathFlag>(1 << emitter->data->_bitForUnusedFlag)) != NULL)
    {
        RenderStateSetArg arg = {
            .emitter = emitter,
            .renderer = this,
            .flushCache = true,
            .argData = argData,
        };
        system->GetDrawPathRenderStateSetCallback(static_cast<DrawPathFlag>(1 << emitter->data->_bitForUnusedFlag))(arg);
    }

    if (!draw)
        return;

    if (!gpuCalc && (!isChild && data->flags & 0x200))
    {
        u32 i = 0;

        PtclViewZ* sortedPtcls = static_cast<PtclViewZ*>(AllocFromDoubleBuffer(sizeof(PtclViewZ) * numParticles));
        if (sortedPtcls != NULL)
        {
            PtclInstance* ptcl = emitter->particleHead;
            if (ptcl == NULL)
                return;

            while (ptcl != NULL)
            {
                sortedPtcls[i].ptcl = ptcl;
                sortedPtcls[i].z = view.m[2][0] * ptcl->worldPos.x + view.m[2][1] * ptcl->worldPos.y + view.m[2][2] * ptcl->worldPos.z + view.m[2][3];
                sortedPtcls[i].idx = i;

                ptcl = ptcl->next; i++;
            }

            qsort(sortedPtcls, i, sizeof(PtclViewZ), ComparePtclViewZ);

            for (u32 j = 0; j < i; j++)
            {
                BindParticleAttributeBlock(&emitter->ptclAttributeBuffer[sortedPtcls[j].idx], shader, 0, 1);

                if (meshType == MeshType_Primitive && primitive != NULL)
                    GX2DrawIndexed(static_cast<GX2PrimitiveType>(primitiveMode), primitive->numIndex, GX2_INDEX_FORMAT_U32, primitive->vbIndex.buffer);

                else
                    GX2Draw(static_cast<GX2PrimitiveType>(primitiveMode), 4);
            }

            this->numDrawParticle += i;
        }
    }
    else if (!gpuCalc)
    {
        u32 numDrawParticle;
        PtclAttributeBuffer* ptclAttributeBuffer;

        if (!isChild)
        {
            numDrawParticle = emitter->numDrawParticle;
            ptclAttributeBuffer = emitter->ptclAttributeBuffer;
        }

        else
        {
            numDrawParticle = emitter->numDrawChildParticle;
            ptclAttributeBuffer = emitter->childPtclAttributeBuffer;
        }

        DrawCpuEntry(shader, primitiveMode, 0, numDrawParticle, ptclAttributeBuffer, primitive);

        this->numDrawParticle += numDrawParticle;
    }
    else if (shader->vertexShaderKey.flags[0] & 0x8000000)
    {
        BindGpuParticleAttributeBlock(emitter->ptclAttributeBufferGpu, shader, 0, emitter->numDrawParticle);

        bool posBind = const_cast<EmitterInstance*>(emitter)->posStreamOutAttributeBuffer.Bind(shader->attrStreamOutPosBuffer, 0, emitter->swapStreamOut, false);
        bool vecBind = const_cast<EmitterInstance*>(emitter)->vecStreamOutAttributeBuffer.Bind(shader->attrStreamOutVecBuffer, 1, emitter->swapStreamOut, false);

        if (!posBind || !vecBind)
            return;

        DrawGpuEntry(shader, primitiveMode, 0, emitter->numDrawParticle, NULL, primitive);

        this->numDrawParticle += emitter->numDrawParticle;
    }
    else if (emitter->controller->emissionRatio < 1.0f && !emitter->controller->emissionRatioChanged)
    {
        s32 max = (u32)((f32)emitter->numPtclAttributeBufferGpuMax * emitter->controller->emissionRatio);
        if (emitter->currentPtclAttributeBufferGpuIdx < emitter->numDrawParticle && emitter->currentPtclAttributeBufferGpuIdx < max)
            DrawGpuEntry(shader, primitiveMode, emitter->numPtclAttributeBufferGpuMax - (max - emitter->currentPtclAttributeBufferGpuIdx), max - emitter->currentPtclAttributeBufferGpuIdx, emitter->ptclAttributeBufferGpu, primitive);

        DrawGpuEntry(shader, primitiveMode, 0, emitter->currentPtclAttributeBufferGpuIdx, emitter->ptclAttributeBufferGpu, primitive);

        this->numDrawParticle += emitter->numDrawParticle;
    }
    else
    {
        DrawGpuEntry(shader, primitiveMode, 0, emitter->numDrawParticle, emitter->ptclAttributeBufferGpu, primitive);

        this->numDrawParticle += emitter->numDrawParticle;
    }

    numDrawEmitter++;
}

bool Renderer::EntryChildParticleSub(const EmitterInstance* emitter, void* argData, bool draw)
{
    if (emitter->numDrawChildParticle == 0 || emitter->childEmitterDynamicUniformBlock == NULL || emitter->childPtclAttributeBuffer == NULL || !draw)
        return false;

    ParticleShader* shader = emitter->childShader[shaderType];
    if (shader == NULL)
        return false;

    const ChildData* childData = emitter->GetChildData();

    if (!SetupParticleShaderAndVertex(shader, childData->meshType, emitter->childPrimitive))
        return false;

    {
        const EmitterStaticUniformBlock* emitterStaticUniformBlock = emitter->childEmitterStaticUniformBlock;
        shader->vertexEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
        shader->fragmentEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
    }

    {
        const EmitterDynamicUniformBlock* emitterDynamicUniformBlock = emitter->childEmitterDynamicUniformBlock;
        shader->vertexEmitterDynamicUniformBlock.BindUniformBlock(emitterDynamicUniformBlock);
        shader->fragmentEmitterDynamicUniformBlock.BindUniformBlock(emitterDynamicUniformBlock);
    }

    SetupTexture(shader, &childData->texture, NULL, NULL);

    shader->EnableInstanced();
    RequestParticle(emitter, shader, true, argData, draw);
    shader->DisableInstanced();

    return true;
}

bool Renderer::EntryParticleSub(const EmitterInstance* emitter, void* argData, bool draw)
{
    if (emitter->numDrawParticle == 0 || emitter->emitterDynamicUniformBlock == NULL || !draw)
        return false;

    ParticleShader* shader = emitter->shader[shaderType];
    if (shader == NULL)
        return false;

    if (!(shader->vertexShaderKey.flags[0] & 0x2000000))
    {
        if (emitter->ptclAttributeBuffer == NULL)
            return false;
    }
    else
    {
        if (emitter->ptclAttributeBufferGpu == NULL)
            return false;
    }

    const SimpleEmitterData* data = emitter->data;

    if (!SetupParticleShaderAndVertex(shader, data->meshType, emitter->primitive))
        return false;

    {
        const EmitterStaticUniformBlock* emitterStaticUniformBlock = emitter->emitterStaticUniformBlock;
        shader->vertexEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
        shader->fragmentEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
    }

    {
        const EmitterDynamicUniformBlock* emitterDynamicUniformBlock = emitter->emitterDynamicUniformBlock;
        shader->vertexEmitterDynamicUniformBlock.BindUniformBlock(emitterDynamicUniformBlock);
        shader->fragmentEmitterDynamicUniformBlock.BindUniformBlock(emitterDynamicUniformBlock);
    }

    SetupTexture(shader, &data->textures[0], &data->textures[1], &data->textures[2]);

    shader->EnableInstanced();
    RequestParticle(emitter, shader, false, argData, draw);
    shader->DisableInstanced();

    return true;
}

void Renderer::EntryParticle(const EmitterInstance* emitter, void* argData)
{
    if (viewUniformBlock == NULL)
        return;

    primitive = NULL;

    CustomShaderDrawOverrideCallback callback = system->GetCustomShaderDrawOverrideCallback(static_cast<CustomShaderCallBackID>(emitter->data->shaderUserSetting));
    ShaderDrawOverrideArg arg = {
        .emitter = emitter,
        .renderer = this,
        .flushCache = true,
        .argData = argData,
    };

    if (!(emitter->controller->renderVisibilityFlags & renderVisibilityFlags))
        return;

    bool stripe = false;
    if (emitter->data->vertexTransformMode == VertexTransformMode_Stripe
        || emitter->data->vertexTransformMode == VertexTransformMode_Complex_Stripe)
    {
        EntryStripe(emitter, argData);
        stripe = true;
    }

    if (emitter->data->type == EmitterType_Complex)
    {
        bool hasChild = emitter->HasChild();

        const ComplexEmitterData* cdata = emitter->GetComplexEmitterData();
        CustomShaderDrawOverrideCallback childCallback = NULL;

        if (hasChild)
            childCallback = system->GetCustomShaderDrawOverrideCallback(static_cast<CustomShaderCallBackID>(emitter->GetChildData()->shaderUserSetting));

        if (hasChild && cdata->childFlags & 0x1000)
        {
            currentParticleType = PtclType_Child;

            if (childCallback != NULL)
                childCallback(arg);

            else
                EntryChildParticleSub(emitter, argData);
        }

        if (cdata->displayParent != 0 && !stripe)
        {
            currentParticleType = PtclType_Complex;

            if (callback != NULL)
                callback(arg);

            else
                EntryParticleSub(emitter, argData);
        }

        if (hasChild && !(cdata->childFlags & 0x1000))
        {
            currentParticleType = PtclType_Child;

            if (childCallback != NULL)
                childCallback(arg);

            else
                EntryChildParticleSub(emitter, argData);
        }
    }
    else
    {
        if (emitter->data->displayParent != 0 /* && !stripe */)
        {
            currentParticleType = PtclType_Simple;

            if (callback != NULL)
                callback(arg);

            else
                EntryParticleSub(emitter, argData);
        }
    }

    currentParticleType = PtclType_Max;
}

void Renderer::BeginStremOut()
{
    GX2SetRasterizerClipControl(GX2_DISABLE, GX2_ENABLE);
    GX2SetStreamOutEnable(GX2_ENABLE);
}

void Renderer::CalcStremOutParticle(const EmitterInstance* emitter, bool bind)
{
    if (emitter->numDrawParticle == 0 || emitter->emitterDynamicUniformBlock == NULL)
        return;

    ParticleShader* shader = emitter->shader[ShaderType_Normal];
    if (shader == NULL)
        return;

    if (!(shader->vertexShaderKey.flags[0] & 0x2000000))
    {
        if (emitter->ptclAttributeBuffer == NULL)
            return;
    }
    else
    {
        if (emitter->ptclAttributeBufferGpu == NULL)
            return;
    }

    const SimpleEmitterData* data = emitter->data;

    if (!SetupParticleShaderAndVertex(shader, data->meshType, emitter->primitive))
        return;

    {
        const EmitterStaticUniformBlock* emitterStaticUniformBlock = emitter->emitterStaticUniformBlock;
        shader->vertexEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
        shader->fragmentEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
    }

    {
        const EmitterDynamicUniformBlock* emitterDynamicUniformBlock = emitter->emitterDynamicUniformBlock;
        shader->vertexEmitterDynamicUniformBlock.BindUniformBlock(emitterDynamicUniformBlock);
        shader->fragmentEmitterDynamicUniformBlock.BindUniformBlock(emitterDynamicUniformBlock);
    }

    shader->EnableInstanced();

    BindGpuParticleAttributeBlock(emitter->ptclAttributeBufferGpu, shader, 0, emitter->numDrawParticle);

    if (bind)
    {
        bool posBind = const_cast<EmitterInstance*>(emitter)->posStreamOutAttributeBuffer.Bind(shader->attrStreamOutPosBuffer, 0, emitter->swapStreamOut, true);
        bool vecBind = const_cast<EmitterInstance*>(emitter)->vecStreamOutAttributeBuffer.Bind(shader->attrStreamOutVecBuffer, 1, emitter->swapStreamOut, true);

        if (!posBind || !vecBind)
            return;
    }

    GX2DrawEx(GX2_PRIMITIVE_POINTS, 1, 0, emitter->numDrawParticle);

    const_cast<EmitterInstance*>(emitter)->posStreamOutAttributeBuffer.UnBind(0);
    const_cast<EmitterInstance*>(emitter)->vecStreamOutAttributeBuffer.UnBind(1);

    const_cast<EmitterInstance*>(emitter)->posStreamOutAttributeBuffer.UnBind(0);
    const_cast<EmitterInstance*>(emitter)->vecStreamOutAttributeBuffer.UnBind(1);

    shader->DisableInstanced();
}

void Renderer::EndStremOut()
{
    GX2SetStreamOutEnable(GX2_DISABLE);
    GX2SetRasterizerClipControl(GX2_ENABLE, GX2_ENABLE);
}

void Renderer::EndRender()
{
}

} } // namespace nw::eft
