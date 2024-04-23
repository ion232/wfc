#include "benchmark.h"
#include "wfc_factory.h"

#include <chrono>
#include <iostream>
#include <string>
#include <tuple>

namespace app {

Benchmark::Benchmark(WfcFactory&& wfc_factory)
    : m_wfc_factory(std::move(wfc_factory))
{}

void Benchmark::run(const std::vector<Parameters>& parameters_list) {
    std::cout << "Image: " << m_wfc_factory.image_path() << std::endl;

    for (const auto& parameters : parameters_list) {
        std::cout << parameters.width << "x" << parameters.height << " ";

        if (auto elapsed = solve_time(parameters)) {
            std::cout << "t=" << std::to_string(elapsed->count()) << "ms";
        } else {
            // ion232: This could be replaced with more granular data.
            std::cout << "Failed.";
        }

        std::cout << std::endl;
    }
}

std::optional<std::chrono::milliseconds> Benchmark::solve_time(const Parameters& parameters) {
    using namespace std::chrono;

    auto wfc = m_wfc_factory.make_wfc(parameters.width, parameters.height);
    auto image = m_wfc_factory.image_model();

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
