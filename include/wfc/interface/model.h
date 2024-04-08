#pragma once

#include "wfc/id.h"
#include "wfc/domain.h"

#include <unordered_set>
#include <unordered_map>

namespace wfc::model::interface {

class Model {
public:
    using Adjacencies = std::vector<std::unordered_map<Id, std::size_t>>;
    using Weights = IdMap<std::size_t>;

    virtual ~Model() {};
    virtual Adjacencies lookup(Id id) = 0;
    virtual std::size_t adjacent_count() = 0;
    virtual Weights weights() = 0;
    virtual Domain initial_domain() = 0;
};

} // namespace wfc::model
