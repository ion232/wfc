#include "gui.h"

#include <iostream>
#include <optional>

namespace app {
namespace {
    void log(const std::string& message) {
        std::cout << "[app] " << message << std::endl;
    }

    void log_sdl_error() {
        log("SDL error: " + std::string(SDL_GetError()));
    }

    SDL_Window* make_window(std::size_t width, std::size_t height) {
        auto* window = SDL_CreateWindow(
            "ion232/wfc_app",
            static_cast<int>(width),
            static_cast<int>(height),
            SDL_WINDOW_ALWAYS_ON_TOP
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

        if (SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED) != 0) {
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

Gui::Gui(WfcFactory&& wfc_factory)
    : m_wfc_factory(std::move(wfc_factory))
{}

int Gui::run(std::size_t width, std::size_t height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        log_sdl_error();
        return EXIT_FAILURE;
    }

    auto* window = make_window(width, height);
    if (window == nullptr) {
        log("Failed to make window.");
        return EXIT_FAILURE;
    }

    for (;;) {
        if (!solve(window)) {
            break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

bool Gui::solve(SDL_Window* window) {
    auto* surface = SDL_GetWindowSurface(window);

    if (surface == nullptr) {
        log("Window surface is null.");
        return false;
    }

    auto* screen_pixels = static_cast<uint32_t*>(surface->pixels);

    if (screen_pixels == nullptr) {
        log("Screen pixels are null.");
        return false;
    }

    auto width = int(0);
    auto height = int(0);

    if (SDL_GetWindowSizeInPixels(window, &width, &height) != 0) {
        return false;
    }

    auto wfc = m_wfc_factory.make_wfc(width, height);
    auto image_model = m_wfc_factory.image_model();
    auto pixels = image_model->make_pixels(wfc.variables());
    auto sdl_event = SDL_Event();
    auto running = wfc::Wfc::Result::Ok;

    while (running == wfc::Wfc::Result::Ok) {
        running = wfc.step();

        while (SDL_PollEvent(&sdl_event)) {
            if  (sdl_event.type == SDL_EVENT_QUIT) {
                return false;
            }
        }

        if (SDL_LockSurface(surface) != 0) {
            continue;
        }

        pixels = image_model->make_pixels(wfc.variables());

        for (int y = 0; y < width; y++) {
            for (int x = 0; x < height; x++) {
                const auto index = static_cast<std::size_t>(x + (y * width));
                screen_pixels[index] = wfc::image::argb(pixels[index]);
            }
        }

        SDL_Delay(1);
        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(window);
    }

    return true;
}

} // namespace app
