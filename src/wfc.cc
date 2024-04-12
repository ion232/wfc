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
        auto connected_indices = m_variables.indices_at_degrees_from(current_index);
        m_propagation_stack.pop();

        for (std::size_t i = 0; i < connected_indices.size(); i++) {
            auto& index = connected_indices[i];
            if (!index) {
                continue;
            }
            auto& variable = m_variables[*index];

            if (auto changed = variable.constrain_to(possible_ids[i])) {
                if (variable.size() == 0) {
                    return true;
                }
                if (variable.size() == 1) {
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
        m_config.model->constraint_degrees(),
        IdSet(domain_ids.capacity() + 1)
    );

    for (const auto& id : domain_ids) {
        const auto& constraints_for_id = m_config.model->constraints(id);
        for (std::size_t i = 0; i < constraints_for_id.size(); i++) {
            for (const auto& [id, count] : constraints_for_id[i]) {
                std::ignore = count;
                constraining_ids[i].insert(id);
            }
        }
    }

    return constraining_ids;
}

bool Wfc::resolved() {
    return m_variables_assigned == m_variables.size();
}

} // namespace wfc
