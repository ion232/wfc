#pragma once

#include "wfc/variable.h"
#include "wfc/id.h"

namespace wfc::heuristic::interface {

class Variable {
public:
    virtual ~Variable() {};
    virtual void inform(std::size_t index, const wfc::Variable& variable) = 0;
    virtual std::optional<std::size_t> pick_variable() = 0;
};

} // namespace wfc::heuristic::interface
