#pragma once

#include "wfc/id.h"
#include "wfc/domain.h"

#include <cstddef>
#include <unordered_map>

namespace wfc::heuristic {

class Weights {
private:
    std::unordered_map<Id, std::size_t> m_weights;

public:
    Weights() = delete;
    Weights(std::unordered_map<Id, std::size_t>&& weights);

    std::initializer_list<std::size_t> weights(const Domain& domain);
};

} // namespace wfc::heuristic 
