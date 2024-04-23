#pragma once

#include "wfc_factory.h"

#include "wfc/wfc.h"

#include <chrono>
#include <filesystem>
#include <utility>
#include <optional>
#include <vector>

namespace app {

class Benchmark {
private:
    WfcFactory m_wfc_factory;

public:
    struct Parameters {
        std::size_t width;
        std::size_t height;
    };

    Benchmark(WfcFactory&& wfc_factory);

    void run(const std::vector<Parameters>& parameters_list);

private:
    std::optional<std::chrono::milliseconds> solve_time(const Parameters& parameters);
};

} // namespace app
