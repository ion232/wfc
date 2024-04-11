#pragma once

#include "wfc/id.h"
#include "wfc/variable.h"

#include <unordered_set>
#include <unordered_map>

namespace wfc::model::interface {

class Model {
public:
    using Constraints = std::vector<IdMap<std::size_t>>;
    using Weights = IdMap<std::size_t>;

    virtual ~Model() {};

    virtual Variable make_variable() = 0;
    virtual Constraints& constraints(Id id) = 0;
    virtual std::size_t constraint_degrees() const noexcept = 0;
    virtual Weights weights() = 0;
};

} // namespace wfc::model
