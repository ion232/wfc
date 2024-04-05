#include "wfc/wfc.h"

#include <algorithm>

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
    if (resolved()) {
        return true;
    }

    constrain(); 
    propagate();

    return false;
}

const data::Matrix<Domain>& Wfc::variables() {
    return m_variables;
}

void Wfc::constrain() {
    auto index = m_config.variable_heuristic->pick_variable();
    if (!index) {
        return;
    }
    auto& domain = m_variables[*index];
    auto id = m_config.assignment_heuristic->choose_from(domain.ids());
    domain.assign(id);

    m_variables_assigned++;
    m_config.variable_heuristic->inform(*index, domain);
    m_propagation_stack.push(std::move(*index));
}

void Wfc::propagate() {
    while (!m_propagation_stack.empty()) {
        const auto& current_index = m_propagation_stack.top();
        auto& current_domain = m_variables[current_index];
        auto adjacent_indices = m_variables.adjacent(current_index);
        auto valid_ids = valid_adjacencies(current_domain.ids());

        for (auto& index : adjacent_indices) {
            if (!index) {
                continue;
            }

            auto& domain = m_variables[*index];
            if (auto changed = domain.constrain_to(valid_ids[*index])) {
                m_variables_assigned += (changed && domain.size() == 1);
                m_propagation_stack.emplace(index);
            }
        }
    }
}

std::vector<std::unordered_set<Id>> Wfc::valid_adjacencies(const Domain::Set& ids) {
    auto adjacencies = std::vector<Domain::Set>();
    for (std::size_t i = 0; i < m_config.model->adjacent_count(); i++) {
        std::ignore = adjacencies.emplace_back();
    }

    for (const auto& id : ids) {
        auto valid_adjacent_ids = m_config.model->lookup(id);
        for (std::size_t i = 0; i < valid_adjacent_ids.size(); i++) {
            adjacencies[i].emplace(valid_adjacent_ids[i]);
        }
    }

    return adjacencies;
}

bool Wfc::resolved() {
    return m_variables_assigned == m_variables.size();
}

} // namespace wfc
