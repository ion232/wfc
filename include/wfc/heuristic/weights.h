#pragma once

#include "wfc/id.h"
#include "wfc/domain.h"

#include <unordered_map>

namespace wfc::heuristic {

class Weights {
private:
    std::unordered_map<Id, float> m_weights;

public:
    Weights() = delete;
    Weights(std::unordered_map<Id, float>&& weights);

    std::initializer_list<float> weights(const Domain& domain);
};

} // namespace wfc::heuristic 
