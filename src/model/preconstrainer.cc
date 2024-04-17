#include "wfc/model/preconstrainer.h"

namespace wfc::model {

void Preconstrainer::add(data::Coordinate coordinate, const Predicate& predicate) {
    m_preconstraints[coordinate] = predicate;
}

void Preconstrainer::preconstrain(const Image::Patterns& patterns, data::Tensor<Variable>& variables) {
    for (const auto& [coordinate, preconstraint] : m_preconstraints) {
        auto& variable = variables[coordinate];
        const auto variable_ids = variable.ids();

        for (const auto& id : variable_ids) {
            if (!preconstraint(patterns.at(id))) {
                variable.remove(id);
            } 
        }
    }
}

} // namespace wfc::model
