#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_System.h>

namespace nw { namespace eft {

EmitterInstance* EmitterSet::GetAliveEmitter(u32 idx)
{
    u32 aliveIdx = 0;

    for (s32 i = 0; i < numEmitterAtCreate; i++)
        if (emitters[i]->emitterSetCreateID == createID && emitters[i]->calc != NULL)
        {
            if (aliveIdx == idx)
                return emitters[i];

            aliveIdx++;
        }

    return NULL;
}

void EmitterSet::SetMtx(const math::MTX34& matrixSRT)
{
    this->matrixSRT = matrixSRT;

    scaleForMatrix.x = ((math::VEC3){ matrixSRT.m[0][0], matrixSRT.m[1][0], matrixSRT.m[2][0] }).Magnitude();
    scaleForMatrix.y = ((math::VEC3){ matrixSRT.m[0][1], matrixSRT.m[1][1], matrixSRT.m[2][1] }).Magnitude();
    scaleForMatrix.z = ((math::VEC3){ matrixSRT.m[0][2], matrixSRT.m[1][2], matrixSRT.m[2][2] }).Magnitude();

    if (scaleForMatrix.x > 0.0f)
    {
        f32 xInv = 1.0f / scaleForMatrix.x;
        matrixRT.m[0][0] = matrixSRT.m[0][0] * xInv;
        matrixRT.m[1][0] = matrixSRT.m[1][0] * xInv;
        matrixRT.m[2][0] = matrixSRT.m[2][0] * xInv;
    }
    else
    {
        matrixRT.m[0][0] = 0.0f;
        matrixRT.m[1][0] = 0.0f;
        matrixRT.m[2][0] = 0.0f;
    }

    if (scaleForMatrix.y > 0.0f)
    {
        f32 yInv = 1.0f / scaleForMatrix.y;
        matrixRT.m[0][1] = matrixSRT.m[0][1] * yInv;
        matrixRT.m[1][1] = matrixSRT.m[1][1] * yInv;
        matrixRT.m[2][1] = matrixSRT.m[2][1] * yInv;
    }
    else
    {
        matrixRT.m[0][1] = 0.0f;
        matrixRT.m[1][1] = 0.0f;
        matrixRT.m[2][1] = 0.0f;
    }

    if (scaleForMatrix.z > 0.0f)
    {
        f32 zInv = 1.0f / scaleForMatrix.z;
        matrixRT.m[0][2] = matrixSRT.m[0][2] * zInv;
        matrixRT.m[1][2] = matrixSRT.m[1][2] * zInv;
        matrixRT.m[2][2] = matrixSRT.m[2][2] * zInv;
    }
    else
    {
        matrixRT.m[0][2] = 0.0f;
        matrixRT.m[1][2] = 0.0f;
        matrixRT.m[2][2] = 0.0f;
    }

    matrixRT.m[0][3] = matrixSRT.m[0][3];
    matrixRT.m[1][3] = matrixSRT.m[1][3];
    matrixRT.m[2][3] = matrixSRT.m[2][3];

    ptclEffectiveScale.x = ptclScale.x * scaleForMatrix.x;
    ptclEffectiveScale.y = ptclScale.y * scaleForMatrix.y;
}

void EmitterSet::Kill()
{
    system->KillEmitterSet(this);
}

void EmitterSet::Fade()
{
    doFade = 1;

    for (s32 i = 0; i < numEmitterAtCreate; i++)
        if (emitters[i] != NULL && emitters[i]->calc != NULL)
            emitters[i]->fadeStartFrame = emitters[i]->counter;
}

} } // namespace nw::eft
