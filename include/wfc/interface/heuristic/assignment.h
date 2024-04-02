#pragma once

#include "wfc/domain.h"
#include "wfc/id.h"

#include <unordered_set>

namespace wfc::heuristic::interface {

class Assignment {
public:
    virtual ~Assignment() {};
    virtual Id assign(const Domain::Set& ids) = 0;
};

} // namespace wfc::heuristic::interface
