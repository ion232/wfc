#pragma once

#include "wfc/variable.h"
#include "wfc/data/tensor.h"
#include "wfc/model/image.h"
#include "wfc/model/overlap/pattern.h"

#include <map>

namespace wfc::model {

class Preconstrainer {
public:
    using Predicate = std::function<bool(const overlap::Pattern&)>;

private:
    std::map<data::Coordinate, Predicate> m_preconstraints;

public:
    void add(data::Coordinate coordinate, const Predicate& predicate);
    void preconstrain(const Image::Patterns& patterns, data::Tensor<Variable>& variables);
};

} // namespace wfc::model
