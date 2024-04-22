#include "gui.h"

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

Gui::Gui(wfc::Wfc&& wfc, std::shared_ptr<wfc::model::Image> image_model)
    : m_wfc(std::move(wfc))
    , m_image_model(std::move(image_model))
{}

int Gui::run() {
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
    auto running = wfc::Wfc::Result::Ok;

    while (running == wfc::Wfc::Result::Ok) {
        running = m_wfc.step();

        while (SDL_PollEvent(&sdl_event)) {
            if  (sdl_event.type == SDL_EVENT_QUIT) {
                running = wfc::Wfc::Result::Resolved;
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

Gui make_gui(Components&& components) {
    auto& [wfc, image] = components;
    auto gui = Gui(std::move(wfc), image);

    return gui;
}

} // namespace app
