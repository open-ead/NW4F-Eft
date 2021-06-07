#include <eft_Config.h>
#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
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

Renderer::Renderer(Heap* argHeap, System* argSystem, const Config& config)
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

    heap = argHeap;

    depthBufferTexture = NULL;
    frameBufferTexture = NULL;

    currentParticleType = 3;
    shaderType = ShaderType_Normal;
    _178 = 0x3F;

    math::VEC3* pos = static_cast<nw::math::VEC3*>(vbPos.AllocateVertexBuffer(argHeap, sizeof(nw::math::VEC3) * 4, 3));
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
    vbPos.Invalidate();

    u32* index = static_cast<u32*>(vbIndex.AllocateVertexBuffer(argHeap, sizeof(u32) * 4, 1));
    index[0] = 0;
    index[1] = 1;
    index[2] = 2;
    index[3] = 3;
    vbIndex.Invalidate();

    primitive = NULL;

    u32 doubleBufferSize = config.doubleBufferSize;
    doubleBufferSize += config.numParticleMax *  0xCC;
    doubleBufferSize += config.numEmitterMax  * 0x114;

    doubleBuffer.Initialize(heap, doubleBufferSize);
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
        viewUniformBlock->viewParam.z = 0.0f;
        viewUniformBlock->viewParam.w = 0.0f;

        GX2EndianSwap(viewUniformBlock, sizeof(ViewUniformBlock));
        GX2Invalidate(GX2_INVALIDATE_CPU_UNIFORM_BLOCK, viewUniformBlock, sizeof(ViewUniformBlock));
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
                primitive->vbTexCoord.BindBuffer(shader->attrTexCoordBuffer, primitive->vbTexCoord.bufferSize, sizeof(math::VEC2));
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

void Renderer::RequestParticle(const EmitterInstance* emitter, ParticleShader* shader, bool isChild, bool flushCache, void* argData)
{
    const SimpleEmitterData* data = emitter->data;

    u32 numParticles = emitter->numParticles;
    const TextureRes* texture0 = &data->textures[0];
    const TextureRes* texture1 = &data->textures[1];
    CustomShaderCallBackID callbackID = static_cast<CustomShaderCallBackID>(data->shaderUserSetting);
    ZBufATestType zBufATestType = data->zBufATestType;
    BlendType blendType = data->blendType;
    DisplaySideType displaySideType = data->displaySideType;
    MeshType meshType = data->meshType;

    if (isChild)
    {
        const ChildData* childData = emitter->GetChildData();

        numParticles = emitter->numChildParticles;
        texture0 = &childData->texture;
        texture1 = NULL;
        callbackID = static_cast<CustomShaderCallBackID>(childData->shaderUserSetting);
        zBufATestType = childData->zBufATestType;
        blendType = childData->blendType;
        displaySideType = childData->displaySideType;
        meshType = childData->meshType;
    }

    if (numParticles == 0)
        return;

    renderContext.SetupZBufATest(zBufATestType);
    renderContext.SetupBlendType(blendType);
    renderContext.SetupDisplaySideType(displaySideType);

    renderContext.SetupTexture(texture0, TextureSlot_0, shader->fragmentSamplerLocations[0]);

    if (texture1 != NULL && texture1->initialized != 0)
        renderContext.SetupTexture(texture1, TextureSlot_1, shader->fragmentSamplerLocations[1]);
    else
        renderContext.SetupTexture((const TextureRes*)NULL, TextureSlot_1, (FragmentTextureLocation){ 0u });

    if (depthBufferTexture != NULL && shader->fragmentDepthBufferSamplerLocation.location != 0xFFFFFFFF)
        renderContext.SetupTexture(depthBufferTexture, TextureSlot_Depth_Buffer, shader->fragmentDepthBufferSamplerLocation);

    if (frameBufferTexture != NULL && shader->fragmentFrameBufferSamplerLocation.location != 0xFFFFFFFF)
        renderContext.SetupTexture(frameBufferTexture, TextureSlot_Frame_Buffer, shader->fragmentFrameBufferSamplerLocation);

    if (system->GetCustomShaderRenderStateSetCallback(callbackID) != NULL)
    {
        RenderStateSetArg arg = {
            .emitter = emitter,
            .renderer = this,
            .flushCache = flushCache,
            .argData = argData,
        };
        system->GetCustomShaderRenderStateSetCallback(callbackID)(arg);
    }

    if (!isChild && data->flags & 0x200)
    {
        u32 i = 0;

        PtclViewZ* sortedPtcls = static_cast<PtclViewZ*>(AllocFromDoubleBuffer(sizeof(PtclViewZ) * numParticles));
        if (sortedPtcls != NULL)
        {
            PtclInstance* ptcl = emitter->particleHead;
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
                BindParticleAttributeBlock(&emitter->ptclAttributeBuffer[sortedPtcls[j].idx], shader, 1);

                if (meshType == MeshType_Primitive && primitive != NULL)
                    GX2DrawIndexed(static_cast<GX2PrimitiveType>(primitiveMode), primitive->numIndex, GX2_INDEX_FORMAT_U32, primitive->vbIndex.buffer);

                else
                    GX2Draw(static_cast<GX2PrimitiveType>(primitiveMode), 4);
            }
        }
    }
    else
    {
        u32 numDrawParticle;

        if (!isChild)
        {
            numDrawParticle = emitter->numDrawParticle;
            BindParticleAttributeBlock(emitter->ptclAttributeBuffer, shader, numDrawParticle);
        }

        else
        {
            numDrawParticle = emitter->numDrawChildParticle;
            BindParticleAttributeBlock(emitter->childPtclAttributeBuffer, shader, numDrawParticle);
        }

        if (meshType == MeshType_Primitive && primitive != NULL)
            GX2DrawIndexedEx(static_cast<GX2PrimitiveType>(primitiveMode), primitive->numIndex, GX2_INDEX_FORMAT_U32, primitive->vbIndex.buffer, 0, numDrawParticle);

        else
            GX2DrawEx(static_cast<GX2PrimitiveType>(primitiveMode), 4, 0, numDrawParticle);
    }
}

void Renderer::EntryChildParticleSub(const EmitterInstance* emitter, bool flushCache, void* argData)
{
    ParticleShader* shader = emitter->childShader[shaderType];
    if (shader == NULL
        || emitter->childPtclAttributeBuffer == NULL
        || emitter->childEmitterDynamicUniformBlock == NULL
        || !SetupParticleShaderAndVertex(shader, emitter->GetChildData()->meshType, emitter->childPrimitive))
    {
        return;
    }

    {
        const EmitterStaticUniformBlock* emitterStaticUniformBlock = emitter->childEmitterStaticUniformBlock;
        shader->vertexEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
        shader->fragmentEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
    }

    shader->vertexEmitterDynamicUniformBlock.BindUniformBlock(emitter->childEmitterDynamicUniformBlock);

    shader->EnableInstanced();
    RequestParticle(emitter, shader, true, flushCache, argData);
    shader->DisableInstanced();
}

void Renderer::EntryParticleSub(const EmitterInstance* emitter, bool flushCache, void* argData)
{
    ParticleShader* shader = emitter->shader[shaderType];
    if (shader == NULL
        || emitter->ptclAttributeBuffer == NULL
        || emitter->emitterDynamicUniformBlock == NULL
        || !SetupParticleShaderAndVertex(shader, emitter->data->meshType, emitter->primitive))
    {
        return;
    }

    {
        const EmitterStaticUniformBlock* emitterStaticUniformBlock = emitter->emitterStaticUniformBlock;
        shader->vertexEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
        shader->fragmentEmitterStaticUniformBlock.BindUniformBlock(emitterStaticUniformBlock);
    }

    shader->vertexEmitterDynamicUniformBlock.BindUniformBlock(emitter->emitterDynamicUniformBlock);

    shader->EnableInstanced();
    RequestParticle(emitter, shader, false, flushCache, argData);
    shader->DisableInstanced();
}

void Renderer::EntryParticle(EmitterInstance* emitter, bool flushCache, void* argData)
{
    if (viewUniformBlock == NULL)
        return;

    CustomShaderDrawOverrideCallback callback = system->GetCustomShaderDrawOverrideCallback(static_cast<CustomShaderCallBackID>(emitter->data->shaderUserSetting));
    ShaderDrawOverrideArg arg = {
        .emitter = emitter,
        .renderer = this,
        .flushCache = flushCache,
        .argData = argData,
    };

    if (!(emitter->controller->_C & _178))
        return;

    bool stripe = false;
    if (emitter->data->vertexTransformMode == VertexTransformMode_Stripe
        || emitter->data->vertexTransformMode == VertexTransformMode_Complex_Stripe)
    {
        EntryStripe(emitter, flushCache, argData);
        stripe = true;
    }

    if (emitter->data->type == EmitterType_Complex && emitter->HasChild())
    {
        const ComplexEmitterData* cdata = emitter->GetComplexEmitterData();
        CustomShaderDrawOverrideCallback childCallback = system->GetCustomShaderDrawOverrideCallback(static_cast<CustomShaderCallBackID>(emitter->GetChildData()->shaderUserSetting));

        if (cdata->childFlags & 0x1000 && emitter->numDrawChildParticle != 0 && emitter->childPtclAttributeBuffer != NULL)
        {
            currentParticleType = 2;

            if (childCallback != NULL)
                childCallback(arg);

            else
                EntryChildParticleSub(emitter, flushCache, argData);
        }

        if (cdata->displayParent != 0 && !stripe && emitter->numDrawParticle != 0 && emitter->ptclAttributeBuffer != NULL)
        {
            currentParticleType = 1;

            if (callback != NULL)
                callback(arg);

            else
                EntryParticleSub(emitter, flushCache, argData);
        }

        if (!(cdata->childFlags & 0x1000) && emitter->numDrawChildParticle != 0 && emitter->childPtclAttributeBuffer != NULL)
        {
            currentParticleType = 2;

            if (childCallback != NULL)
                childCallback(arg);

            else
                EntryChildParticleSub(emitter, flushCache, argData);
        }
    }
    else
    {
        if (emitter->data->displayParent != 0 && !stripe && emitter->numDrawParticle != 0 && emitter->ptclAttributeBuffer != NULL)
        {
            currentParticleType = 0;

            if (callback != NULL)
                callback(arg);

            else
                EntryParticleSub(emitter, flushCache, argData);
        }
    }

    currentParticleType = 3;
}

void Renderer::EndRender()
{
}

} } // namespace nw::eft
