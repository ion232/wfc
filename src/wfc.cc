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
        auto valid_ids = valid_adjacencies(current_variable.ids());
        auto adjacent_indices = m_variables.adjacent(current_index);
        m_propagation_stack.pop();

        for (std::size_t i = 0; i < adjacent_indices.size(); i++) {
            auto& index = adjacent_indices[i];
            if (!index) {
                continue;
            }
            auto& variable = m_variables[*index];

            if (auto changed = variable.constrain_to(valid_ids[i])) {
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

std::vector<IdSet> Wfc::valid_adjacencies(const IdSet& ids) {
    auto adjacencies = std::vector<IdSet>();
    adjacencies.reserve(m_config.model->adjacency_count());

    for (const auto& id : ids) {
        const auto& valid_adjacent_ids = m_config.model->adjacencies(id);
        adjacencies.emplace_back(ids.capacity() - 1);
        for (std::size_t i = 0; i < valid_adjacent_ids.size(); i++) {
            for (const auto& [valid_id, count] : valid_adjacent_ids[i]) {
                std::ignore = count;
                adjacencies[i].insert(valid_id);
            }
        }
    }

    return adjacencies;
}

bool Wfc::resolved() {
    return m_variables_assigned == m_variables.size();
}

} // namespace wfc
