#pragma once

#include "wfc/domain.h"
#include "wfc/id.h"

#include <optional>
#include <unordered_set>

namespace wfc::heuristic::interface {

class Assignment {
public:
    virtual ~Assignment() {};
    virtual std::optional<Id> choose_from(const IdSet& ids) = 0;
};

} // namespace wfc::heuristic::interface
