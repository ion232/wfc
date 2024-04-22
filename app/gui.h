#pragma once

#include "components.h"
#include "wfc/wfc.h"
#include "wfc/model/image.h"

namespace app {

class Gui {
private:
    wfc::Wfc m_wfc;
    std::shared_ptr<wfc::model::Image> m_image_model;

public:
    Gui(wfc::Wfc&& wfc, std::shared_ptr<wfc::model::Image> image_model);

    int run();
};

Gui make_gui(Components&& components);

} // namespace app
