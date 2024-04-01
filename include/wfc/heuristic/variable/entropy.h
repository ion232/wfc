#pragma once

#include "wfc/interface/heuristic/variable.h"

#include <set>
#include <utility>

namespace wfc::heuristic::variable {

class Entropy: public interface::Variable {
private:
    using Entry = std::pair<std::size_t, float>;

    std::set<Entry> m_undecided;

public:
    void inform(std::size_t index, const Domain& domain) override;

    std::size_t pick_variable() override;
};

} // namespace wfc::heuristic::variable
