#include "app.h"
#include "png_loader.h"

#include "wfc/model/image_factory.h"
#include "wfc/heuristic/assignment/sample.h"
#include "wfc/heuristic/choice/entropy.h"

#include <iostream>

#include <SDL3/SDL.h>

namespace app {
namespace {
    void log_sdl_error() {
        std::cout << "SDL error: " << SDL_GetError() << std::endl;
    }

    SDL_Window* make_window(int width, int height) {
        static constexpr auto no_window_flags = SDL_WindowFlags(0);

        auto* window = SDL_CreateWindow("ion232/wfc", width, height, no_window_flags);
        if (window == nullptr) {
            log_sdl_error();
            return nullptr;
        }

        auto* screen_surface = SDL_GetWindowSurface(window);
        if (screen_surface == nullptr) {
            log_sdl_error();
            return nullptr;
        }

        const auto pixel_color = SDL_MapRGB(screen_surface->format, 0xAA, 0xAA, 0xAA);
        if (SDL_FillSurfaceRect(screen_surface, nullptr, pixel_color) != 0) {
            log_sdl_error();
            return nullptr;
        }

        if (SDL_UpdateWindowSurface(window) != 0) {
            log_sdl_error();
            return nullptr;
        }

        return window;
    }
}

App::App(wfc::Wfc&& wfc, std::shared_ptr<wfc::model::Image> image_model)
    : m_wfc(std::move(wfc))
    , m_image_model(std::move(image_model))
{}

int App::run() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        log_sdl_error();
        return EXIT_FAILURE;
    }

    const auto dimensions = m_wfc.variables().dimensions();
    const auto width = dimensions[0];
    const auto height = dimensions[1];

    auto* window = make_window(width, height);
    if (window == nullptr) {
        return EXIT_FAILURE;
    }

    auto* surface = SDL_GetWindowSurface(window);
    if (surface == nullptr) {
        return EXIT_FAILURE;
    }

    auto* screen_pixels = static_cast<uint32_t*>(surface->pixels);
    if (screen_pixels == nullptr) {
        return EXIT_FAILURE;
    }

    const auto& variables = m_wfc.variables();
    auto pixels = m_image_model->make_pixels(variables);
    auto sdl_event = SDL_Event();
    auto running = true;

    while (running) {
        running = !m_wfc.step();

        while (SDL_PollEvent(&sdl_event)) {
            if  (sdl_event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        if (SDL_LockSurface(surface) != 0) {
            continue;
        }

        pixels = m_image_model->make_pixels(variables);

        for (std::size_t y = 0; y < width; y++) {
            for (std::size_t x = 0; x < height; x++) {
                const auto index = x + (y * width);
                screen_pixels[index] = pixels[index];
            }
        }

        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

App make(
    const std::filesystem::path& image_path,
    std::size_t width,
    std::size_t height
)
{
    auto image = [&image_path](){
        auto png_loader = std::make_shared<app::PngLoader>();
        auto image_factory =  wfc::model::ImageFactory(png_loader);
        return image_factory.make_image(image_path);
    }();

    auto config = [&image](){
        auto random = std::make_shared<wfc::io::Random>();
        auto weights = std::make_shared<wfc::heuristic::Weights>(image->weights());
        auto sample = std::make_shared<wfc::heuristic::assignment::Sample>(weights, random);
        auto entropy = std::make_shared<wfc::heuristic::choice::Entropy>(weights, random);
        return wfc::Wfc::Config{sample, entropy, image, random};
    }();

    auto dimensions = std::vector<std::size_t>({width, height});
    auto variable = image->make_variable();
    auto tensor = wfc::data::Tensor<wfc::Variable>(
        std::move(dimensions),
        std::move(variable)
    );

    auto wfc = wfc::Wfc(std::move(config), std::move(tensor));
    auto app = app::App(std::move(wfc), image);

    return app;
}

} // namespace app
