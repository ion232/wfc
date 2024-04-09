#include "wfc/wfc.h"

#include <algorithm>
#include <iostream>

namespace wfc {

Wfc::Wfc(Config&& config, data::Tensor<Domain>&& variables)
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
    //  static auto steps = std::size_t(0);
    
    //  std::cout << "Steps: " << std::to_string(steps++) << std::endl;

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

data::Tensor<Domain>& Wfc::variables() {
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
    //  std::cout << "Assigned id " << std::to_string(*id) << " to index " << std::to_string(*index) << std::endl;

    m_variables_assigned++;
    m_config.variable_heuristic->inform(*index, domain);
    m_propagation_stack.push(std::move(*index));

    return false;
}

bool Wfc::propagate() {
    // auto propagated = std::size_t(0);

    while (!m_propagation_stack.empty()) {
        // propagated++;
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
                if (domain.size() == 0) {
                    return true;
                } else if (domain.size() == 1) {
                    m_variables_assigned += 1;
                }
                m_config.variable_heuristic->inform(*index, domain);
                m_propagation_stack.push(std::move(*index));
            }
        }
    }
    // std::cout << "Propagated: " << std::to_string(propagated) << std::endl;

    return false;
}

// TODO: ion232: This could do with being refactored for readability.
std::vector<IdSet> Wfc::valid_adjacencies(const IdSet& ids) {
    static auto adjacencies = std::vector<IdSet>(8, IdSet(300));
    // static auto adjacencies = std::vector<IdSet>(8, IdSet());
    for (auto& id_set : adjacencies) {
        id_set.clear();
    }

    for (const auto& id : ids) {
        const auto& valid_adjacent_ids = m_config.model->lookup(id);
        for (std::size_t i = 0; i < valid_adjacent_ids.size(); i++) {
            const auto& valid_ids = valid_adjacent_ids[i];
            for (const auto& [valid_id, count] : valid_ids) {
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
