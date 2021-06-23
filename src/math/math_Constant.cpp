#include <math/math_Constant.h>

#include <limits>

namespace nw { namespace math {

const f32 F_PI = 3.1415927410125732f;
const f32 F_ULP = std::numeric_limits<f32>::epsilon() * 2.0f;
const u32 I_HALF_ROUND_IDX = 0x80000000;

} } // namespace nw::math
