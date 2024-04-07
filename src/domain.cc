#include "wfc/domain.h"

#include <iostream>
#include <vector>

namespace wfc {

Domain::Domain()
    : m_set()
{}

Domain::Domain(Id max_id)
    : m_set()
{
    for (Id id = 0; id < max_id; id++) {
        m_set.emplace(id);
    }
}

void Domain::assign(Id id) {
    m_set = IdSet({id});
}

bool Domain::constrain_to(const IdSet& allowed) {
    auto to_remove = std::vector<Id>();

    for (const auto& id : m_set) {
        if (!allowed.contains(id)) {
            to_remove.emplace_back(id);
        }
    }

    for (const auto& id : to_remove) {
        m_set.erase(id);
    }

    auto changed = to_remove.size() > 0;
    return changed;
}

IdSet Domain::ids() const noexcept {
    return m_set;
}

std::size_t Domain::size() const noexcept {
    return m_set.size();
}

} // namespace wfc
