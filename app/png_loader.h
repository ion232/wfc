#include "wfc/interface/image/loader.h"

#include "lodepng.h"

namespace wfc::image {

class PngLoader: public interface::Loader {
public:
    PngLoader::Pixels load_pixels(const std::filesystem::path& path) override;
};

} // namespace wfc::image
