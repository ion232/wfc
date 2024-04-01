#include "wfc/heuristic/assignment/sample.h"

namespace wfc::heuristic::assignment {

Sample::Sample(
    std::shared_ptr<Weights> weights,
    std::shared_ptr<io::Random> random
)
    : m_weights(std::move(weights))
    , m_random(std::move(random))
{}

Id Sample::assign(const Domain::Set& ids) {
    auto offset = m_random->make_size_t(0, ids.size());
    auto id = *std::next(ids.begin(), offset);

    return id;
}

} // namespace wfc::heuristic::assignment 
