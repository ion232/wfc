#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <SDL3/SDL.h>

const auto SCREEN_WIDTH = int(640);
const auto SCREEN_HEIGHT = int(480);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }

    static constexpr auto no_window_flags = SDL_WindowFlags(0);
    auto* window = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, no_window_flags);
    if (window == nullptr) {
        printf("SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    auto* screenSurface = SDL_GetWindowSurface(window);
    if (screenSurface == nullptr) {
        printf("SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    const auto pixel_color = SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF);
    if (SDL_FillSurfaceRect(screenSurface, nullptr, pixel_color) != 0) {
        printf("SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_UpdateWindowSurface(window) != 0) {
        printf("SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    auto sdl_event = SDL_Event();
    auto running = true;

    while (running) {
        while (SDL_PollEvent(&sdl_event)){
            if (sdl_event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
