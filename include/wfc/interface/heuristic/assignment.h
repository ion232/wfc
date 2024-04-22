#pragma once

#include "wfc/id.h"
#include "wfc/id_set.h"
#include "wfc/variable.h"

#include <optional>
#include <unordered_set>

namespace wfc::heuristic::interface {

class Assignment {
public:
    virtual ~Assignment() {};
    virtual std::optional<Id> assign_from(const IdSet& ids) = 0;
};

} // namespace wfc::heuristic::interface
