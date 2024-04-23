#include "wfc/model/preconstrainer.h"

#include <iostream>

namespace wfc::model {

void Preconstrainer::add(data::Coordinate coordinate, const Predicate& predicate) {
    m_preconstraints[coordinate] = predicate;
}

void Preconstrainer::preconstrain(const Image::Patterns& patterns, data::Tensor<Variable>& variables) {
    for (const auto& [coordinate, preconstraint] : m_preconstraints) {
        auto& variable = variables[coordinate];
        const auto domain = variable.domain();

        for (const auto& id : domain) {
            if (!preconstraint(patterns.at(id))) {
                variable.remove(id);
            } 
        }
    }
}

} // namespace wfc::model
