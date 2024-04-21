#include "wfc/variable.h"

#include <iostream>
#include <vector>

namespace wfc {

Variable::Variable()
    : m_ids()
{}

Variable::Variable(IdSet&& supports)
    : m_ids(std::move(supports))
{}

void Variable::assign(Id id) {
    m_ids.clear();
    m_ids.set(id);
}

void Variable::remove(Id id) {
    m_ids.unset(id);
}

bool Variable::constrain_to(const IdSet& allowed) {
    const auto size_before = m_ids.size();
    m_ids.intersect_with(allowed);

    const auto size_after = m_ids.size();
    return size_before != size_after;
}

IdSet Variable::ids() const noexcept {
    return m_ids;
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
    return m_ids.size();
}

bool Variable::operator==(const Variable& other) const {
    return m_ids == other.m_ids;
}

} // namespace wfc
