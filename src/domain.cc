#include "wfc/domain.h"

#include <iostream>
#include <vector>

namespace wfc {

Domain::Domain()
    : m_map()
{}

Domain::Domain(IdMap<std::size_t>&& map)
    : m_map(std::move(map))
{}

void Domain::assign(Id id) {
    m_map = IdMap<std::size_t>({{id, m_map[id]}});
}

bool Domain::constrain_to(const IdSet& allowed) {
    auto to_remove = std::vector<Id>();

    for (auto& [id, count] : m_map) {
        if (allowed.contains(id)) {
            continue;
        }

        count--;

        if (count == 0) {
            to_remove.emplace_back(id);
        }
    }

    for (const auto& id : to_remove) {
        m_map.erase(id);
    }

    auto changed = to_remove.size() > 0;
    return changed;
}

IdSet Domain::ids() const noexcept {
    auto ids = IdSet(300);
    for (const auto& [id, count] : m_map) {
        if (count != 0) {
            ids.insert(id);
        }
    }
    return ids;
}

std::size_t Domain::size() const noexcept {
    return m_map.size();
}

bool Domain::operator==(const Domain& other) const {
    return m_map == other.m_map;
}

} // namespace wfc
