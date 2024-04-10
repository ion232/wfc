#pragma once

#include "wfc/wfc.h"
#include "wfc/model/image.h"

namespace app {

class App {
private:
    wfc::Wfc m_wfc;
    std::shared_ptr<wfc::model::Image> m_image_model;

public:
    App(wfc::Wfc&& wfc, std::shared_ptr<wfc::model::Image> image_model);

    int run();
};

App make(
    const std::filesystem::path& image_path,
    std::size_t width,
    std::size_t height
);

} // namespace app
