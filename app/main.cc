#include "app.h"

#include <iostream>

#include <SDL3/SDL.h>

static const auto base_image_path = std::filesystem::path("./assets/images");

int main(int argc, const char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: ./app image_name width height" << std::endl;
        return;
    }

    auto app = [argv](){
        auto image_file_name = std::string(argv[1]) + ".png";
        auto image_path = base_image_path / image_file_name;
        auto width = std::stoull(argv[2]);
        auto height = std::stoull(argv[3]);
        return app::make(image_file_name, width, height);
    }();

    auto result = app.run();

    return result;
}
