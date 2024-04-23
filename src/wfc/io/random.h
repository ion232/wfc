#pragma once

#include <random>

namespace wfc::io {

class Random {
private:
    std::random_device m_device;
    std::seed_seq m_seed_seq;
    std::mt19937 m_engine;

public:
    Random();
    Random(std::int32_t seed);

    std::size_t make_size_t(std::size_t min, std::size_t max);
    std::size_t sample_index(const std::vector<std::size_t>& weights);
    float make_float(float min, float max);
};

} // namespace wfc::io
