#include "wfc/heuristic/assignment/sample.h"

namespace wfc::heuristic::assignment {

Sample::Sample(
    std::shared_ptr<Weights> weights,
    std::shared_ptr<io::Random> random
)
    : m_weights(std::move(weights))
    , m_random(std::move(random))
{}

Id Sample::choose_from(const Domain::Set& ids) {
    const auto weights = m_weights->of(ids);

    auto total = std::size_t(0);
    for (const auto& w : weights) {
        total += w;
    }

    const auto random_int = m_random->make_size_t(0, total);
    auto it = ids.begin();
    auto id = 0;
    total = 0;

    // TODO: ion232: Use binary search or see if there is a better method.
    for (std::size_t i = 0; i < ids.size(); i++) {
        total += weights[i];
        if (random_int <= total) {
            return *it;
        }
        it = std::next(it);
    } 

    return 0;
}

} // namespace wfc::heuristic::assignment 
