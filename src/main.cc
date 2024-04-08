#include "wfc/wfc.h"
#include "wfc/model/image.h"
#include "wfc/heuristic/assignment/sample.h"
#include "wfc/heuristic/variable/entropy.h"

#include <iostream>
#include <optional>

#include <SDL3/SDL.h>

static constexpr auto screen_width = int(256);
static constexpr auto screen_height = int(256);

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

    const auto pixel_color = SDL_MapRGB(screen_surface->format, 0xAA, 0xAA, 0xAA);
    if (SDL_FillSurfaceRect(screen_surface, nullptr, pixel_color) != 0) {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    if (SDL_UpdateWindowSurface(window) != 0) {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    auto sdl = SDL{window};

    return sdl;
}

int main() {
    using namespace wfc;
    
    static constexpr auto output_width = screen_width;
    static constexpr auto output_height = screen_height;
    
    const auto image_path = std::filesystem::path("/Users/ion/Repos/wfc/assets/images/flowers.png");
    auto image = std::make_shared<model::Image>(std::move(*model::load_image(image_path)));
    auto config = [image](){
        const auto seed = std::int32_t(1337);
        auto random = std::make_shared<io::Random>(seed);
        auto weights = std::make_shared<heuristic::Weights>(image->weights());
        auto sample = std::make_shared<heuristic::assignment::Sample>(weights, random);
        auto entropy = std::make_shared<heuristic::variable::Entropy>(weights, random);
        return Wfc::Config{sample, entropy, image, random};
    }();
    auto dimensions = std::vector<std::size_t>({output_width, output_height});
    auto matrix = data::Matrix<Domain>(
                                       std::move(dimensions),
                                       Domain(image->initial_domain())
                                       );
    auto wfc = Wfc(std::move(config), std::move(matrix));
    
    auto running = true;
//        while (running) {
//            running =
//        }
//        running = true;
    


    auto sdl = setup_sdl();
    if (!sdl) {
        return 1;
    }
    
    auto* surface = SDL_GetWindowSurface(sdl->window);
    auto sdl_event = SDL_Event();

    while (running) {
        if (!wfc.step() && SDL_LockSurface(surface) == 0) {
            const auto& xs = wfc.variables();
            auto pixels = image->make_pixels(xs);
            auto* screen_pixels = static_cast<uint32_t*>(surface->pixels);
            for (int y = 0; y < output_width; y++) {
                for (int x = 0; x < output_height; x++) {
                    screen_pixels[y * screen_width + x] = pixels[y * output_width + x];
                }
            }

            SDL_UnlockSurface(surface);
        }
        
        SDL_UpdateWindowSurface(sdl->window);

         while (SDL_PollEvent(&sdl_event)){
             if  (sdl_event.type == SDL_EVENT_QUIT) {
                 running = false;
             }
         }

         SDL_Delay(1);
     }

     SDL_DestroyWindow(sdl->window);
     SDL_Quit();

    return 0;
}
