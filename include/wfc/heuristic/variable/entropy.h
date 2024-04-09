#pragma once

#include "wfc/heuristic/weights.h"
#include "wfc/io/random.h"
#include "wfc/interface/heuristic/variable.h"

#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

namespace wfc::heuristic::variable {

// TODO: ion232: Need to find a more performant way of doing this. This will do for now.
class Entropy: public interface::Variable {
private:
    std::shared_ptr<Weights> m_weights;
    std::shared_ptr<io::Random> m_random;
    std::unordered_map<std::size_t, float> m_undecided;

public:
    Entropy() = delete;
    Entropy(
        std::shared_ptr<Weights> weights,
        std::shared_ptr<io::Random> random
    );

    void inform(std::size_t index, const wfc::Variable& variable) override;

    std::optional<std::size_t> pick_variable() override;
};

} // namespace wfc::heuristic::variable
