#pragma once

#include "wfc/heuristic/weights.h"
#include "wfc/interface/heuristic/assignment.h"

#include <memory>

namespace wfc::heuristic::assignment {

class Sample: public interface::Assignment {
private:
    std::shared_ptr<Weights> m_weights;
    std::shared_ptr<io::Random> m_random;

public:
    Sample() = delete;
    Sample(
        std::shared_ptr<Weights> weights,
        std::shared_ptr<io::Random> random
    );

    std::optional<Id> choose_from(const IdSet& ids) override;
};

} // namespace wfc::heuristic::assignment 
