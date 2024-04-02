#pragma once

#include "wfc/heuristic/weights.h"
#include "wfc/interface/heuristic/variable.h"

#include <optional>
#include <set>
#include <utility>

namespace wfc::heuristic::variable {

class Entropy: public interface::Variable {
private:
    using Entry = std::pair<std::size_t, float>;

    std::shared_ptr<Weights> m_weights;
    std::set<Entry> m_undecided;

public:
    Entropy() = delete;
    Entropy(std::shared_ptr<Weights> weights);

    void inform(std::size_t index, const Domain& domain) override;

    std::optional<std::size_t> pick_variable() override;
};

} // namespace wfc::heuristic::variable
