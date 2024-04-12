#include "wfc/heuristic/weights.h"

namespace wfc::heuristic {

Weights::Weights(IdMap<std::size_t>&& weights)
    : m_weights(std::move(weights))
{}

std::vector<std::size_t> Weights::of(const IdSet& ids) {
    auto weights = std::vector<std::size_t>();
    for (const auto& id : ids) {
        weights.emplace_back(m_weights[id]);
    }
    return weights;
}

} // namespace wfc::heuristic 
