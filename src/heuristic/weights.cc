#include "wfc/heuristic/weights.h"

namespace wfc::heuristic {

Weights::Weights(std::unordered_map<Id, std::size_t>&& weights)
    : m_weights(std::move(weights))
{}

std::initializer_list<std::size_t> Weights::weights(const Domain& domain) {
    return {};
}

} // namespace wfc::heuristic 
