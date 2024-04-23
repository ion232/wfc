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

Wfc::Result Wfc::step() {
    if (resolved()) {
        return Result::Resolved;
    }

    if (auto result = constrain(); result != Result::Ok) {
        return result;
    }

    if (auto result = propagate(); result != Result::Ok) {
        return result;
    }

    return Result::Ok;
}

data::Tensor<Variable>& Wfc::variables() {
    return m_variables;
}

Wfc::Result Wfc::constrain() {
    auto index = m_config.choice_heuristic->choose();

    if (!index) {
        return Result::InvalidChoice;
    }

    auto& variable = m_variables[*index];
    auto id = m_config.assignment_heuristic->assign_from(variable.domain());

    if (!id) {
        return Result::InvalidAssignment;
    }

    variable.assign(*id);
    m_variables_assigned++;
    m_config.choice_heuristic->inform(*index, variable);
    m_propagation_stack.push(std::move(*index));

    return Result::Ok;
}

Wfc::Result Wfc::propagate() {
    while (!m_propagation_stack.empty()) {
        const auto& current_index = m_propagation_stack.top();
        auto& current_variable = m_variables[current_index];
        auto possible_domains = constraining_domains(current_variable.domain());
        auto offset_indices = m_variables.indices_at_offsets_from(current_index);

        m_propagation_stack.pop();

        for (std::size_t i = 0; i < offset_indices.size(); i++) {
            auto& index = offset_indices[i];

            if (!index) {
                continue;
            }

            auto& variable = m_variables[*index];

            if (auto changed = variable.constrain_to(possible_domains[i]); changed) {
                const auto state = variable.state();

                if (state == Variable::State::Invalid) {
                    return Result::Contradiction;
                } else if (state == Variable::State::Decided) {
                    m_variables_assigned++;
                }

                m_config.choice_heuristic->inform(*index, variable);
                m_propagation_stack.push(std::move(*index));
            }
        }
    }

    return Result::Ok;
}

std::vector<Variable::Domain> Wfc::constraining_domains(const Variable::Domain& target_domain) {
    auto domains = std::vector<Variable::Domain>(
        m_config.model->constraint_offsets(),
        Variable::Domain(target_domain.capacity(), false)
    );

    for (const auto& id : target_domain) {
        const auto& constraints_for_id = m_config.model->constraints(id);

        for (std::size_t i = 0; i < constraints_for_id.size(); i++) {
            domains[i].union_with(constraints_for_id[i]);
        }
    }

    return domains;
}

bool Wfc::resolved() {
    return m_variables_assigned == m_variables.size();
}

} // namespace wfc
