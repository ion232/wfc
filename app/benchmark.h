#pragma once

#include "components.h"
#include "wfc/wfc.h"

#include <chrono>
#include <filesystem>
#include <utility>
#include <optional>
#include <vector>

namespace app {

class Benchmark {
private:
    std::filesystem::path m_image_path;

public:
    struct Parameters {
        std::size_t width;
        std::size_t height;
    };

    Benchmark(const std::filesystem::path& image_path);

    void run(const std::vector<Parameters>& parameters_list);

private:
    std::optional<std::chrono::milliseconds> solve_time(const Parameters& parameters);
};

} // namespace app
