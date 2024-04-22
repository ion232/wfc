#pragma once

#include "wfc/id.h"
#include "wfc/id_map.h"
#include "wfc/id_set.h"
#include "wfc/variable.h"

#include <unordered_set>
#include <unordered_map>

namespace wfc::model::interface {

class Model {
public:
    using Constraints = std::vector<IdSet>;
    using Weights = IdMap<std::size_t>;

    virtual ~Model() {};

    virtual data::Tensor<Variable> make_variables(const std::vector<data::Dimension>& dimensions) = 0;
    virtual Constraints& constraints(Id id) = 0;
    virtual std::size_t constraint_offsets() const noexcept = 0;
    virtual Weights weights() = 0;
};

} // namespace wfc::model
