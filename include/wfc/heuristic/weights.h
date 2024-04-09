#pragma once

#include "wfc/id.h"
#include "wfc/variable.h"

#include <cstddef>
#include <unordered_map>

namespace wfc::heuristic {

class Weights {
private:
    std::unordered_map<Id, std::size_t> m_weights;

public:
    Weights() = delete;
    Weights(std::unordered_map<Id, std::size_t>&& weights);

    std::vector<std::size_t> of(const IdSet& ids);
};

} // namespace wfc::heuristic 
