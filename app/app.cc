#include "app.h"
#include "png_loader.h"

#include "wfc/model/image_factory.h"
#include "wfc/model/preconstrainer.h"
#include "wfc/heuristic/assignment/sample.h"
#include "wfc/heuristic/choice/entropy.h"

#include <iostream>

#include <SDL3/SDL.h>

namespace app {
namespace {
    void log(const std::string& message) {
        std::cout << "[app] " << message << std::endl;
    }

    void log_sdl_error() {
        log("SDL error: " + std::string(SDL_GetError()));
    }

    SDL_Window* make_window(std::size_t width, std::size_t height) {
        static constexpr auto no_window_flags = SDL_WindowFlags(0);

        auto* window = SDL_CreateWindow(
            "ion232/wfc_app",
            static_cast<int>(width),
            static_cast<int>(height),
            no_window_flags
        );
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
        log("Failed to make window.");
        return EXIT_FAILURE;
    }

    auto* surface = SDL_GetWindowSurface(window);
    if (surface == nullptr) {
        log("Window surface is null.");
        return EXIT_FAILURE;
    }

    auto* screen_pixels = static_cast<uint32_t*>(surface->pixels);
    if (screen_pixels == nullptr) {
        log("Screen pixels are null.");
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
                screen_pixels[index] = wfc::image::argb(pixels[index]);
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
        auto image_factory = wfc::model::ImageFactory(png_loader);
        auto pattern_dimensions = std::vector<wfc::data::Dimension>({3, 3});
        auto result = image_factory.make_image(image_path, pattern_dimensions);
        return result;
    }();

    auto config = [&image](){
        auto random = std::make_shared<wfc::io::Random>();
        auto weights = std::make_shared<wfc::heuristic::Weights>(image->weights());
        auto sample = std::make_shared<wfc::heuristic::assignment::Sample>(weights, random);
        auto entropy = std::make_shared<wfc::heuristic::choice::Entropy>(weights, random);
        auto result = wfc::Wfc::Config{sample, entropy, image, random};
        return result;
    }();

    auto preconstraints = [&width, &height, &image_path](){
        static constexpr const auto has_dirt = [](const auto& pattern){
            static const auto dirt = wfc::image::make_pixel(185, 122, 87, 255);
            return pattern.contains(dirt);
        };

        auto result = wfc::model::Preconstrainer();
        if (image_path.filename() != "flowers.png") {
            return result;
        }

        for (std::int32_t x = 0; x < static_cast<std::int32_t>(width); x++) {
            for (std::int32_t y = 0; y < 2; y++) {
                const auto coordinate = wfc::data::Coordinate{x, static_cast<std::int32_t>(height) - y - 1};
                result.add(coordinate, has_dirt);
            }
        }

        return result;
    }();
    auto tensor = [&width, &height, &image, &preconstraints](){
        auto dimensions = std::vector<std::size_t>({width, height});
        auto variables = image->make_variables(dimensions);
        preconstraints.preconstrain(image->patterns(), variables);
        return variables;
    }();

    auto app = [&config, &tensor, &image](){
        auto wfc = wfc::Wfc(std::move(config), std::move(tensor));
        auto result = app::App(std::move(wfc), image);
        return result;
    }();

    return app;
}

} // namespace app
