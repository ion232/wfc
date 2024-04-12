#include "wfc/io/random.h"

namespace wfc::io {
namespace {

std::seed_seq random_seed(std::random_device& device) {
    return std::seed_seq({device()});
}

} // namespace

Random::Random()
    : m_device()
    , m_seed_seq(random_seed(m_device))
    , m_engine(m_seed_seq)
{}

Random::Random(std::int32_t seed)
    : m_device()
    , m_seed_seq({seed})
    , m_engine(m_seed_seq)
{}

std::size_t Random::make_size_t(std::size_t min, std::size_t max) {
    auto distribution = std::uniform_int_distribution<std::size_t>(min, max);
    return distribution(m_engine);
}

float Random::make_float(float min, float max) {
    auto distribution = std::uniform_real_distribution<float>(min, max);
    return distribution(m_engine);
}

} // namespace wfc::io
