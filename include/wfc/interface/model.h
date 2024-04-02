#pragma once

#include "wfc/id.h"

#include <unordered_set>

namespace wfc::model::interface {

class Model {
public:
    virtual ~Model() {};
    virtual std::unordered_set<Id> lookup(Id id) = 0;
    virtual std::unordered_map<Id, std::size_t> weights() = 0;
    virtual std::size_t max_id() = 0;
};

} // namespace wfc::model
