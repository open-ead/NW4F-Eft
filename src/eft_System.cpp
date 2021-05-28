#include <eft_Config.h>
#include <eft_System.h>

namespace nw { namespace eft {

System::System(const Config& config)
    : initialized(false)
{
    if (config.GetHeap())
        Initialize(config.GetHeap(), config);
}

} } // namespace nw::eft
