#pragma once

#include "wfc/interface/image/loader.h"

#include "lodepng.h"

namespace app {

class PngLoader: public wfc::image::interface::Loader {
public:
    PngLoader::Pixels load_pixels(const std::filesystem::path& path) override;
};

} // namespace app
