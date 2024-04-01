#pragma once

#include "wfc/id.h"

#include <unordered_set>

namespace wfc::model::interface {

class Model {
public:
    virtual std::unordered_set<Id> lookup(Id id) = 0;
};

} // namespace wfc::model
