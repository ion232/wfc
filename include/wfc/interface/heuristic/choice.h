#pragma once

#include "wfc/variable.h"
#include "wfc/id.h"

namespace wfc::heuristic::interface {

class Choice {
public:
    virtual ~Choice() {};
    virtual void inform(std::size_t index, const Variable& variable) = 0;
    virtual std::optional<std::size_t> choose() = 0;
};

} // namespace wfc::heuristic::interface
