#include "wfc/variable.h"

#include <iostream>
#include <vector>

namespace wfc {

Variable::Variable()
    : m_domain()
{}

Variable::Variable(Domain&& domain)
    : m_domain(std::move(domain))
{}

void Variable::assign(Id id) {
    m_domain.clear();
    m_domain.set(id);
}

void Variable::remove(Id id) {
    m_domain.unset(id);
}

bool Variable::constrain_to(const Domain& allowed) {
    const auto size_before = m_domain.size();
    m_domain.intersect_with(allowed);

    const auto size_after = m_domain.size();
    return size_before != size_after;
}

Variable::Domain Variable::domain() const noexcept {
    return m_domain;
}

Variable::State Variable::state() const noexcept {
    const auto domain_size = size();

    if (domain_size == 0) {
        return State::Invalid;
    } else if (domain_size == 1) {
        return State::Decided;
    } else {
        return State::Undecided;
    }
}

std::size_t Variable::size() const noexcept {
    return m_domain.size();
}

bool Variable::operator==(const Variable& other) const {
    return m_domain == other.m_domain;
}

} // namespace wfc
