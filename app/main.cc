#include "app.h"

#include <iostream>

#include <SDL3/SDL.h>

int main(int argc, const char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: ./app image_path width height" << std::endl;
        return EXIT_FAILURE;
    }

    auto app = [argv](){
        auto image_path = std::string(argv[1]);
        auto width = std::stoull(argv[2]);
        auto height = std::stoull(argv[3]);
        return app::make(image_path, width, height);
    }();

    auto result = app.run();

    return result;
}
