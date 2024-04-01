#include "wfc/heuristic/variable/entropy.h"

namespace wfc::heuristic::variable {

void Entropy::inform(std::size_t index, const Domain& domain) {
    std::ignore = index;
    std::ignore = domain;
    // if (auto it = m_undecided.find(std::move(entry)); it != m_undecided.end()) {
    //     m_undecided.erase(std::move(it));
    // }
    // m_undecided.insert(entry);
}

std::size_t Entropy::pick_variable() {
    // if (auto it = m_undecided.begin(); it != m_undecided.end()) {
    //     return it->first;
    // }
    return 0;
}

} // namespace wfc::heuristic::variable
