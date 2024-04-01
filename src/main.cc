#include "wfc/wfc.h"
#include "wfc/model/image.h"
#include "wfc/heuristic/assignment/sample.h"
#include "wfc/heuristic/variable/entropy.h"

#include <iostream>
#include <optional>

#include <SDL3/SDL.h>

const auto SCREEN_WIDTH = int(640);
const auto SCREEN_HEIGHT = int(480);

struct SDL {
    SDL_Window* window;
};

std::optional<SDL> setup_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    static constexpr auto no_window_flags = SDL_WindowFlags(0);
    auto* window = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, no_window_flags);
    if (window == nullptr) {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    auto* screen_surface = SDL_GetWindowSurface(window);
    if (screen_surface == nullptr) {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    const auto pixel_color = SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF);
    if (SDL_FillSurfaceRect(screen_surface, nullptr, pixel_color) != 0) {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    if (SDL_UpdateWindowSurface(window) != 0) {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    auto sdl = SDL{ window };

    return sdl;
}

void update_texture(
    SDL_Texture* texture,
    std::vector<uint32_t>&& pixels,
    std::size_t width
)
{
    const auto pitch = int(width * sizeof(uint32_t));
    SDL_UpdateTexture(texture, nullptr, pixels.data(), pitch);
}

int main() {
    using namespace wfc;

    auto sdl = setup_sdl();
    if (!sdl) {
        return 1;
    }

    const auto max_id = std::size_t(9);
    const auto image_width = 100;
    const auto image_height = 100;
    const auto dimensions = std::vector<std::size_t>({image_width, image_height});

    static constexpr auto no_renderer_flags = 0;
    auto* renderer = SDL_CreateRenderer(sdl->window, "PixelRenderer", no_renderer_flags);
    auto* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, image_width, image_height);

    auto image = std::make_shared<model::Image>("./assets/images/forest.png");
    auto config = [image](){
        auto sample = std::make_shared<heuristic::assignment::Sample>();
        auto entropy = std::make_shared<heuristic::variable::Entropy>();
        auto random = std::make_shared<io::Random>(1337);
        return Wfc::Config{sample, entropy, image, random};
    }();
    auto matrix = data::Matrix<Domain>(dimensions, Domain(max_id));
    auto wfc = Wfc(std::move(config), std::move(matrix));

    while (!wfc.step()) {
        auto pixels = image->;
        update_texture(texture, std::move(pixels), image_width);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    }

    // auto sdl_event = SDL_Event();
    // auto running = true;

    // while (running) {
    //     while (SDL_PollEvent(&sdl_event)){
    //         if (sdl_event.type == SDL_EVENT_QUIT) {
    //             running = false;
    //         }
    //     }
    //     SDL_Delay(1);
    // }

    // SDL_DestroyWindow(window);
    // SDL_Quit();

    return 0;
}
