#include "wfc/wfc.h"

#include <algorithm>
#include <iostream>

namespace wfc {

Wfc::Wfc(Config&& config, data::Matrix<Domain>&& variables)
    : m_config(std::move(config))
    , m_variables(std::move(variables))
    , m_variables_assigned(0)
    , m_propagation_stack()
{
    for (std::size_t i = 0; i < m_variables.size(); i++) {
        m_config.variable_heuristic->inform(i, m_variables[i]);
    }
}

bool Wfc::step() {
    static auto steps = std::size_t(0);
    
    std::cout << "Steps: " << std::to_string(steps++) << std::endl;

    if (resolved()) {
        return true;
    }

    if (constrain()) {
        return true;
    }

    propagate();

    return false;
}

data::Matrix<Domain>& Wfc::variables() {
    return m_variables;
}

bool Wfc::constrain() {
    auto index = m_config.variable_heuristic->pick_variable();
    if (!index) {
        return true;
    }
    auto& domain = m_variables[*index];
    auto id = m_config.assignment_heuristic->choose_from(domain.ids());
    if (!id) {
        return true;
    }
    domain.assign(*id);
    std::cout << "Assigned: " << std::to_string(*id) << std::endl;

    m_variables_assigned++;
    m_config.variable_heuristic->inform(*index, domain);
    m_propagation_stack.push(std::move(*index));

    return false;
}

void Wfc::propagate() {
    while (!m_propagation_stack.empty()) {
        const auto& current_index = m_propagation_stack.top();
        auto& current_domain = m_variables[current_index];
        auto adjacent_indices = m_variables.adjacent(current_index);
        auto valid_ids = valid_adjacencies(current_domain.ids());
        m_propagation_stack.pop();

        for (std::size_t i = 0; i < adjacent_indices.size(); i++) {
            auto& index = adjacent_indices[i];
            if (!index) {
                continue;
            }

            auto& domain = m_variables[*index];
            if (auto changed = domain.constrain_to(valid_ids[i])) {
                m_variables_assigned += (changed && domain.size() == 1);
                m_propagation_stack.push(std::move(*index));
            }
        }
    }
}

// TODO: ion232: This could do with being refactored for readability.
std::vector<std::unordered_set<Id>> Wfc::valid_adjacencies(const IdSet& ids) {
    auto adjacencies = std::vector<IdSet>();
    for (std::size_t i = 0; i < m_config.model->adjacent_count(); i++) {
        std::ignore = adjacencies.emplace_back();
    }

    for (const auto& id : ids) {
        auto valid_adjacent_ids = m_config.model->lookup(id);
        for (std::size_t i = 0; i < valid_adjacent_ids.size(); i++) {
            auto& valid_ids = valid_adjacent_ids[i];
            for (auto& valid_id : valid_ids) {
                adjacencies[i].emplace(valid_id);
            }
        }
    }

    return adjacencies;
}

bool Wfc::resolved() {
    return m_variables_assigned == m_variables.size();
}

} // namespace wfc
