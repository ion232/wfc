#include "wfc/heuristic/assignment/sample.h"

#include <random>

namespace wfc::heuristic::assignment {

Sample::Sample(
    std::shared_ptr<Weights> weights,
    std::shared_ptr<io::Random> random
)
    : m_weights(std::move(weights))
    , m_random(std::move(random))
{}

std::optional<Id> Sample::assign_from(const IdSet& ids) {
    const auto weights = m_weights->of(ids);
    const auto random_index = m_random->sample_index(weights);
    auto it = ids.begin();

    for (std::size_t i = 0; i < random_index; i++) {
        it++;
    }

    const auto id = *it;
    return std::make_optional<Id>(std::move(id));
}

} // namespace wfc::heuristic::assignment 
