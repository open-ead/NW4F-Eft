#ifndef EFT_RES_DATA_H_
#define EFT_RES_DATA_H_

#include <types.h>

namespace nw { namespace eft {

struct Header // Actual name not known
{
    char magic[4];
    u32 version;
    u32 numEmitterSet;
    u32 _C;
    u32 strTblOffs;
    u32 textureDataTblOffs;
    u32 textureDataTblSize;
    u32 shaderTblOffs;
    u32 shaderTblSize;
    u32 keyAnimTblOffs;
    u32 keyAnimTblSize;
    u32 primitiveTblOffs;
    u32 primitiveTblSize;
    u32 _34;
    u32 _38;
    u32 _3C;
};

} }

#endif // EFT_RES_DATA_H_
