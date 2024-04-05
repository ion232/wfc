#include "wfc/heuristic/weights.h"

namespace wfc::heuristic {

Weights::Weights(std::unordered_map<Id, std::size_t>&& weights)
    : m_weights(std::move(weights))
{}

std::vector<std::size_t> Weights::of(const Domain::Set& ids) {
    auto weights = std::vector<std::size_t>(m_weights.size(), 0);
    for (const auto& id : ids) {
        weights.emplace_back(m_weights[id]);
    }
    return weights;
}

} // namespace wfc::heuristic 
