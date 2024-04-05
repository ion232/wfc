#include "wfc/wfc.h"
#include "wfc/model/image.h"
#include "wfc/heuristic/assignment/sample.h"
#include "wfc/heuristic/variable/entropy.h"

#include <iostream>
#include <optional>

#include <SDL3/SDL.h>

static constexpr auto screen_width = int(480);
static constexpr auto screen_height = int(480);

struct SDL {
    SDL_Window* window;
};

std::optional<SDL> setup_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    static constexpr auto no_window_flags = SDL_WindowFlags(0);
    auto* window = SDL_CreateWindow("SDL Tutorial", screen_width, screen_height, no_window_flags);
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
    std::vector<std::uint32_t>&& pixels,
    std::size_t width
)
{
    const auto pitch = int(width * sizeof(std::uint32_t));
    SDL_UpdateTexture(texture, nullptr, pixels.data(), pitch);
}

int main() {
    using namespace wfc;

    auto sdl = setup_sdl();
    if (!sdl) {
        return 1;
    }

    static constexpr auto image_width = 480;
    static constexpr auto image_height = 480;
    static constexpr auto no_renderer_flags = 0;

    auto* renderer = SDL_CreateRenderer(sdl->window, "PixelRenderer", no_renderer_flags);
    auto* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        image_width,
        image_height
    );

    const auto image_path = std::filesystem::path("./assets/images/forest.png");
    auto image = std::make_shared<model::Image>(std::move(*model::load_image(image_path)));
    auto config = [image](){
        const auto seed = 1337;
        auto random = std::make_shared<io::Random>(seed);
        auto weights = std::make_shared<heuristic::Weights>(image->weights());
        auto sample = std::make_shared<heuristic::assignment::Sample>(weights, random);
        auto entropy = std::make_shared<heuristic::variable::Entropy>(weights, random);
        return Wfc::Config{sample, entropy, image, random};
    }();
    auto dimensions = std::vector<std::size_t>({image_width, image_height});
    auto matrix = data::Matrix<Domain>(
        std::move(dimensions),
        Domain(image->max_id())
    );
    auto wfc = Wfc(std::move(config), std::move(matrix));

    auto sdl_event = SDL_Event();
    auto running = true;

    while (running) {
        if (!wfc.step()) {
            auto pixels = image->make_pixels(wfc.variables());
            update_texture(texture, std::move(pixels), image_width);
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        }

        while (SDL_PollEvent(&sdl_event)){
            if (sdl_event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();

    return 0;
}
