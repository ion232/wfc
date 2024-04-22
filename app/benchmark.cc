#include "benchmark.h"
#include "components.h"

#include <chrono>
#include <iostream>
#include <string>
#include <tuple>

namespace app {

Benchmark::Benchmark(const std::filesystem::path& path)
    : m_image_path(path)
{}

void Benchmark::run(const std::vector<Parameters>& parameters_list) {
    std::cout << "Image: " << m_image_path << std::endl;

    for (const auto& parameters : parameters_list) {
        std::cout << "Width: " << parameters.width << " ";
        std::cout << "Height: " << parameters.height << " ";

        if (auto elapsed = solve_time(parameters)) {
            std::cout << "Elapsed: " << std::to_string(elapsed->count());
        } else {
            // ion232: This could be replaced with more granular data.
            std::cout << "Failed.";
        }

        std::cout << std::endl;
    }
}

std::optional<std::chrono::milliseconds> Benchmark::solve_time(const Parameters& parameters) {
    using namespace std::chrono;

    auto [wfc, image] = make_components(m_image_path, parameters.width, parameters.height);

    const auto start = steady_clock::now();
    auto wfc_result = wfc.step();

    for (;;) {
        if (wfc_result != wfc::Wfc::Result::Ok) {
            break;
        }

        wfc_result = wfc.step();
    }

    const auto end = steady_clock::now();
    const auto duration = duration_cast<milliseconds>(end - start);
    const auto elapsed = [&wfc_result, &duration](){
        if (wfc_result == wfc::Wfc::Result::Resolved) {
            return std::make_optional(duration);
        } else {
            return std::optional<milliseconds>(std::nullopt);
        }
    }();

    return elapsed;
}

} // namespace app
