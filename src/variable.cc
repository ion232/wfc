#include "wfc/variable.h"

#include <iostream>
#include <vector>

namespace wfc {

Variable::Variable()
    : m_map()
{}

Variable::Variable(IdMap<std::size_t>&& map)
    : m_map(std::move(map))
{}

void Variable::assign(Id id) {
    m_map = IdMap<std::size_t>({{id, m_map[id]}});
}

bool Variable::constrain_to(const IdSet& allowed) {
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

IdSet Variable::ids() const noexcept {
    auto ids = IdSet(300);
    for (const auto& [id, count] : m_map) {
        if (count != 0) {
            ids.insert(id);
        }
    }
    return ids;
}

std::size_t Variable::size() const noexcept {
    return m_map.size();
}

bool Variable::operator==(const Variable& other) const {
    return m_map == other.m_map;
}

} // namespace wfc
