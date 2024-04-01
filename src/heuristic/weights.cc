#include "wfc/heuristic/weights.h"

namespace wfc::heuristic {

Weights::Weights(std::unordered_map<Id, float>&& weights)
    : m_weights(std::move(weights))
{}

std::initializer_list<float> Weights::weights(const Domain& domain) {
    return {};
}

} // namespace wfc::heuristic 
