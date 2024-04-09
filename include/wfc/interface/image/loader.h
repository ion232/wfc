#pragma once

#include "wfc/image/pixels.h"

#include <filesystem>

namespace wfc::image::interface {

class Loader {
public:
    virtual ~Loader() {}

    virtual Pixels load_pixels(const std::filesystem::path& path) = 0;
};

} // namespace wfc::image::interface
