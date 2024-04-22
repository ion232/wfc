#pragma once

#include "wfc/wfc.h"
#include "wfc/model/image.h"

#include <cstddef>
#include <filesystem>

namespace app {

using Components = std::pair<wfc::Wfc, std::shared_ptr<wfc::model::Image>>;

Components make_components(
    const std::filesystem::path& image_path,
    std::size_t width,
    std::size_t height
);

} // namespace app
