#include "wfc/wfc.h"

#include <algorithm>
#include <iostream>

namespace wfc {

Wfc::Wfc(Config&& config, data::Tensor<Variable>&& variables)
    : m_config(std::move(config))
    , m_variables(std::move(variables))
    , m_variables_assigned(0)
    , m_propagation_stack()
{
    for (std::size_t i = 0; i < m_variables.size(); i++) {
        m_config.choice_heuristic->inform(i, m_variables[i]);
    }
}

bool Wfc::step() {
    if (resolved()) {
        return true;
    }

    if (constrain()) {
        return true;
    }

    if (propagate()) {
        return true;
    }

    return false;
}

data::Tensor<Variable>& Wfc::variables() {
    return m_variables;
}

bool Wfc::constrain() {
    auto index = m_config.choice_heuristic->choose();

    if (!index) {
        return true;
    }

    auto& variable = m_variables[*index];
    auto id = m_config.assignment_heuristic->assign_from(variable.ids());

    if (!id) {
        return true;
    }

    variable.assign(*id);
    m_variables_assigned++;
    m_config.choice_heuristic->inform(*index, variable);
    m_propagation_stack.push(std::move(*index));

    return false;
}

bool Wfc::propagate() {
    while (!m_propagation_stack.empty()) {
        const auto& current_index = m_propagation_stack.top();
        auto& current_variable = m_variables[current_index];
        auto possible_ids = constraining_ids(current_variable.ids());
        auto offset_indices = m_variables.indices_at_offsets_from(current_index);

        m_propagation_stack.pop();

        for (std::size_t i = 0; i < offset_indices.size(); i++) {
            auto& index = offset_indices[i];

            if (!index) {
                continue;
            }

            auto& variable = m_variables[*index];

            if (auto changed = variable.constrain_to(possible_ids[i])) {
                const auto state = variable.state();

                if (state == Variable::State::Invalid) {
                    return true;
                } else if (state == Variable::State::Decided) {
                    m_variables_assigned++;
                }

                m_config.choice_heuristic->inform(*index, variable);
                m_propagation_stack.push(std::move(*index));
            }
        }
    }

    return false;
}

std::vector<IdSet> Wfc::constraining_ids(const IdSet& domain_ids) {
    auto constraining_ids = std::vector<IdSet>(
        m_config.model->constraint_offsets(),
        IdSet(domain_ids.capacity(), false)
    );

    for (const auto& id : domain_ids) {
        const auto& constraints_for_id = m_config.model->constraints(id);

        for (std::size_t i = 0; i < constraints_for_id.size(); i++) {
            constraining_ids[i].union_with(constraints_for_id[i]);
        }
    }

    return constraining_ids;
}

bool Wfc::resolved() {
    return m_variables_assigned == m_variables.size();
}

} // namespace wfc
