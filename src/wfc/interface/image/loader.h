#pragma once

#include "wfc/image/pixel.h"

#include <filesystem>
#include <vector>

namespace wfc::image::interface {

class Loader {
public:
    using Pixels = std::vector<std::vector<Pixel>>;

    virtual ~Loader() {}

    virtual Pixels load_pixels(const std::filesystem::path& path) = 0;
};

} // namespace wfc::image::interface
