#include "wfc/wfc.h"

#include <algorithm>

namespace wfc {

Wfc::Wfc(Config&& config, data::Matrix<Domain>&& variables)
    : m_config(std::move(config))
    , m_variables(std::move(variables))
    , m_variables_assigned(0)
    , m_propagation_stack()
{}

bool Wfc::step() {
    if (resolved()) {
        return true;
    }

    observe(); 
    propagate();

    return false;
}

const data::Matrix<Domain>& Wfc::variables() {
    return m_variables;
}

void Wfc::observe() {
    auto index = m_config.variable_heuristic->pick_variable();
    if (!index) {
        return;
    }
    auto& domain = m_variables[*index];
    auto id = m_config.assignment_heuristic->assign(domain.ids());
    domain.choose(id);

    m_variables_assigned++;
    m_config.variable_heuristic->inform(index, domain);
    m_propagation_stack.push(std::move(index));
}

void Wfc::propagate() {
    while (!m_propagation_stack.empty()) {
        const auto& current_index = m_propagation_stack.top();
        auto& current_domain = m_variables[current_index];
        auto adjacent_indices = m_variables.adjacent(current_index);
        auto valid_ids = valid_adjacent(current_domain.ids());

        for (auto& index : adjacent_indices) {
            auto& domain = m_variables[index];
            if (auto changed = domain.constrain_to(valid_ids)) {
                m_variables_assigned += (changed && domain.size() == 1);
                m_propagation_stack.emplace(index);
            }
        }
    }
}

std::unordered_set<Id> Wfc::valid_adjacent(const Domain::Set& ids) {
    auto valid = Domain::Set();

    for (auto id : ids) {
        auto valid_ids = m_config.model->lookup(id);
        for (auto valid_id : valid_ids) {
            std::ignore = valid.insert(valid_id);
        }
    }

    return valid;
}

bool Wfc::resolved() {
    return m_variables_assigned == m_variables.size();
}

} // namespace wfc
