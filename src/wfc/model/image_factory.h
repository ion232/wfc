#pragma once

#include "wfc/data/tensor.h"
#include "wfc/interface/image/loader.h"
#include "wfc/model/image.h"

#include <filesystem>
#include <memory>
#include <optional>

namespace wfc::model {

class ImageFactory {
private:
    std::shared_ptr<image::interface::Loader> m_image_loader;

public:
    ImageFactory(std::shared_ptr<image::interface::Loader> image_loader);

    std::shared_ptr<Image> make_image(
        const std::filesystem::path& path,
        const std::vector<data::Dimension>& pattern_dimensions
    );
};

} // namespace wfc::model
