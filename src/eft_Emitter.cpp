#include <eft_Emitter.h>
#include <eft_ResData.h>

namespace nw { namespace eft {

System* EmitterCalc::mSys = NULL;

void EmitterInstance::Init(const SimpleEmitterData* data)
{
    this->data = data;

    counter = 0.0f;
    counter2 = 0.0f;
    emitCounter = 0.0f;
    emitCounter2 = 0.0f;
    emitLostTime = 0.0f;
    numParticles = 0;
    numChildParticles = 0;

    emissionInterval = data->emitInterval - (s32)random.GetU32(data->emitIntervalRandom);
    fadeAlpha = 1.0f;
    emissionSpeed = 1.0f;

    ptclFollowType = data->ptclFollowType;

    particleHead = NULL;
    childParticleHead = NULL;
    particleTail = NULL;
    childParticleTail = NULL;

    scaleRandom.x = 0.0f;
    scaleRandom.y = 0.0f;
    scaleRandom.z = 0.0f;

    rotateRandom.x = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.x;
    rotateRandom.y = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.y;
    rotateRandom.z = random.GetF32Range(-1.0, 1.0f) * data->emitterRotateRandom.z;

    translateRandom.x = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.x;
    translateRandom.y = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.y;
    translateRandom.z = random.GetF32Range(-1.0, 1.0f) * data->emitterTranslateRandom.z;

    _1EA = 0;

    animArray = static_cast<KeyFrameAnimArray*>(data->keyAnimArray);

    emitLostRate = 0.0f;
    isEmitted = false;
    isCalculated = false;

    for (u32 i = 0; i < ShaderType_Max; i++)
    {
        shader[i] = NULL;
        childShader[i] = NULL;
    }

    primitive = NULL;
    childPrimitive = NULL;

    animMatrixRT = math::MTX34::Identity();
    animMatrixSRT = math::MTX34::Identity();

    prevPosSet = false;
    emitLostDistance = 0.0f;
}

void EmitterInstance::UpdateResInfo()
{
    ptclFollowType = data->ptclFollowType;

    anim[ 0] = data->emissionRate;
    anim[ 1] = data->ptclMaxLifespan;
    anim[15] = data->ptclVelocityMag;
    anim[16] = data->emitterVelocityMag;
    anim[14] = data->emitterAlpha;
    anim[11] = data->emitterColor0.r;
    anim[12] = data->emitterColor0.g;
    anim[13] = data->emitterColor0.b;
    anim[19] = data->emitterColor1.r;
    anim[20] = data->emitterColor1.g;
    anim[21] = data->emitterColor1.b;
    anim[22] = data->emissionShapeScale.x;
    anim[23] = data->emissionShapeScale.y;
    anim[24] = data->emissionShapeScale.z;
    anim[17] = 1.0f;
    anim[18] = 1.0f;
    /* WTF Nintendo
    anim[ 2] = data->emitterScale.x;
    anim[ 3] = data->emitterScale.y;
    anim[ 4] = data->emitterScale.z;
    anim[ 5] = data->emitterRotate.x;
    anim[ 6] = data->emitterRotate.y;
    anim[ 7] = data->emitterRotate.z;
    anim[ 8] = data->emitterTranslate.x;
    anim[ 9] = data->emitterTranslate.y;
    anim[10] = data->emitterTranslate.z;
    */
    anim[ 2] = data->emitterScale.x + scaleRandom.x;
    anim[ 3] = data->emitterScale.y + scaleRandom.y;
    anim[ 4] = data->emitterScale.z + scaleRandom.z;
    anim[ 5] = data->emitterRotate.x + rotateRandom.x;
    anim[ 6] = data->emitterRotate.y + rotateRandom.y;
    anim[ 7] = data->emitterRotate.z + rotateRandom.z;
    anim[ 8] = data->emitterTranslate.x + translateRandom.x;
    anim[ 9] = data->emitterTranslate.y + translateRandom.x;
    anim[10] = data->emitterTranslate.z + translateRandom.x;

    particleBehaviorFlg = 0;
    shaderAvailableAttribFlg = 0;
    childShaderAvailableAttribFlg = 0;

    UpdateEmitterStaticUniformBlock(emitterStaticUniformBlock, data);

    if (data->type != EmitterType_Simple
        && (static_cast<const ComplexEmitterData*>(data)->childFlags & 1))
    {
        const ChildData* childData = NULL;
        if (data->type != EmitterType_Simple
            && (static_cast<const ComplexEmitterData*>(data)->childFlags & 1)) // ???????
        {
            childData = reinterpret_cast<const ChildData*>(static_cast<const ComplexEmitterData*>(data) + 1);
        }

        UpdateChildStaticUniformBlock(childEmitterStaticUniformBlock, childData);
    }
}

} } // namespace nw::eft
