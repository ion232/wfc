#pragma once

#include "wfc/id.h"
#include "wfc/variable.h"

#include <unordered_set>
#include <unordered_map>

namespace wfc::model::interface {

class Model {
public:
    using Adjacencies = std::vector<IdMap<std::size_t>>;
    using Weights = IdMap<std::size_t>;

    virtual ~Model() {};

    virtual Variable initial_variable() = 0;
    virtual Adjacencies& adjacencies(Id id) = 0;
    virtual std::size_t adjacency_count() const noexcept = 0;
    virtual Weights weights() = 0;
};

} // namespace wfc::model
