#pragma once

#include "wfc/domain.h"
#include "wfc/id.h"

namespace wfc::heuristic::interface {

class Variable {
public:
    virtual void inform(std::size_t index, const Domain& domain) = 0;
    virtual std::size_t pick_variable() = 0;
};

} // namespace wfc::heuristic::interface
