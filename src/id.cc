#include "wfc/id.h"

namespace wfc {

IdSet::iterator::iterator(const std::vector<bool>& data, std::size_t index)
    : m_data(data)
    , m_current_index(std::move(index))
{
    if (!m_data.empty() && m_current_index < m_data.size() && !m_data[m_current_index]) {
        advance();
    }
}

IdSet::iterator& IdSet::iterator::operator++() {
    advance();
    return *this;
}

IdSet::iterator IdSet::iterator::operator++(int) {
    auto it = *this;
    advance();
    return it;
}

IdSet::iterator::value_type IdSet::iterator::operator*() const {
    return m_current_index;
}

bool IdSet::iterator::operator==(const IdSet::iterator& other) const {
    return (m_current_index == other.m_current_index);
}

bool IdSet::iterator::operator!=(const IdSet::iterator& other) const {
    return !(*this == other);
}

void IdSet::iterator::advance() {
    if (m_current_index < m_data.size()) {
        m_current_index++;
    }
    while (m_current_index < m_data.size() && !m_data[m_current_index]) {
        m_current_index++;
    }
}

IdSet::IdSet()
    : m_data()
    , m_size(0)
{}

IdSet::IdSet(std::initializer_list<Id> il)
    : m_data(il.size(), false)
    , m_size(0)
{
    for (const auto& id : il) {
        insert(id);
    }
}

IdSet::IdSet(Id max_id)
    : m_data(max_id, false)
    , m_size(0)
{}

void IdSet::insert(const Id& id) {
    if (id >= m_data.size()) {
        m_data.resize(id + 1, false);
    }
    m_size += static_cast<std::size_t>(!m_data[id]);
    m_data[id] = true;
}

void IdSet::remove(const Id& id) {
    if (id >= m_data.size()) {
        return;
    }
    m_size -= static_cast<std::size_t>(m_data[id]);
    m_data[id] = false;
}

void IdSet::intersect_with(const IdSet& id_set) {
    for (std::size_t id = 0; id < m_data.size(); id++) {
        if (!id_set.contains(id)) {
            remove(id);
        }
    }
}

void IdSet::union_with(const IdSet& id_set) {
    for (const auto& id : id_set) {
        insert(id);
    }
}

bool IdSet::contains(const Id& id) const {
    return (id < m_data.size()) && m_data[id];
}

void IdSet::clear() {
    for (std::size_t i = 0; i < m_data.size(); i++) {
        m_data[i] = false;
    }
    m_size = 0;
}

std::size_t IdSet::capacity() const {
    return m_data.size();
}

std::size_t IdSet::size() const {
    return m_size;
}

bool IdSet::operator==(const IdSet& other) const noexcept {
    return m_data == other.m_data;
}

IdSet::iterator IdSet::begin() const {
    return IdSet::iterator(m_data, 0);
}

IdSet::iterator IdSet::end() const {
    return IdSet::iterator(m_data, m_data.size());
}

} // namespace wfc
