#include "benchmark.h"
#include "gui.h"
#include "wfc_factory.h"

#include <iostream>

#include <SDL3/SDL.h>

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./app command image_path [args]" << std::endl;
        return EXIT_FAILURE;
    }

    auto command = std::string(argv[1]);
    auto image_path = std::string(argv[2]);
    auto wfc_factory = app::WfcFactory(image_path);

    try {
        if (command == "gui") {
            auto width = std::stoull(argv[3]);
            auto height = std::stoull(argv[4]);
            auto gui = app::Gui(std::move(wfc_factory));
            auto result = gui.run(width, height);
            return result;
        } else if (command == "benchmark") {
            auto benchmark = app::Benchmark(std::move(wfc_factory));
            benchmark.run({
                {8, 8},
                {16, 16},
                {32, 32},
                {64, 64},
                {128, 128},
                {256, 256}
            });
            return EXIT_SUCCESS;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            return EXIT_FAILURE;
        }
    } catch (const std::exception& e) {
        std::cout << "[app] Exception caught: " << e.what() << std::endl;
    }
}
