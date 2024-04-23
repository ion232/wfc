#pragma once

#include "wfc/wfc.h"
#include "wfc/model/image.h"
#include "wfc_factory.h"

#include <SDL3/SDL.h>

namespace app {

class Gui {
private:
    WfcFactory m_wfc_factory;

public:
    Gui(WfcFactory&& wfc);

    int run(std::size_t width, std::size_t height);

private:
    bool solve(SDL_Window* window);
};

} // namespace app
