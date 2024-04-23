#pragma once

#include "wfc/wfc.h"
#include "wfc/model/image.h"

#include <cstddef>
#include <filesystem>
#include <memory>

namespace app {

class WfcFactory {
private:
    std::filesystem::path m_image_path;
    std::shared_ptr<wfc::model::Image> m_image_model;

public:
    WfcFactory(const std::filesystem::path& image_path);

    wfc::Wfc make_wfc(std::size_t width, std::size_t height);

    std::filesystem::path image_path() const noexcept;
    std::shared_ptr<wfc::model::Image> image_model() const noexcept;
};

} // namespace app
