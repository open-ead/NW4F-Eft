#include <eft_Emitter.h>
#include <eft_EmitterSet.h>
#include <eft_System.h>

namespace nw { namespace eft {

void EmitterController::SetFollowType(PtclFollowType followType)
{
    emitter->ptclFollowType = followType;
}

void EmitterSet::SetMtx(const math::MTX34& matrixSRT)
{
    this->matrixSRT = matrixSRT;

    _204.x = ((math::VEC3){ matrixSRT.m[0][0], matrixSRT.m[1][0], matrixSRT.m[2][0] }).Magnitude();
    _204.y = ((math::VEC3){ matrixSRT.m[0][1], matrixSRT.m[1][1], matrixSRT.m[2][1] }).Magnitude();
    _204.z = ((math::VEC3){ matrixSRT.m[0][2], matrixSRT.m[1][2], matrixSRT.m[2][2] }).Magnitude();

    if (_204.x > 0.0f)
    {
        f32 xInv = 1.0f / _204.x;
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

    if (_204.y > 0.0f)
    {
        f32 yInv = 1.0f / _204.y;
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

    if (_204.z > 0.0f)
    {
        f32 zInv = 1.0f / _204.z;
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

    _220.x = _210.x * _204.x;
    _220.y = _210.y * _204.y;
}

void EmitterSet::Kill()
{
    system->KillEmitterSet(this);
}

} } // namespace nw::eft
