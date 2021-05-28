#ifndef EFT_ENUM_H_
#define EFT_ENUM_H_

namespace nw { namespace eft {

enum CpuCore
{
    CpuCore_0 = 0,
    CpuCore_1 = 1,
    CpuCore_2 = 2,
    CpuCore_Max = 3,
};

enum CustomActionCallBackID
{
    CustomActionCallBackID_Max = 8,
};

enum CustomShaderCallBackID
{
    CustomShaderCallBackID_Max = 9,
};

} } // namespace nw::eft

#endif // EFT_ENUM_H_
