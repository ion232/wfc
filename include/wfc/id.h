#pragma once

#include <cstddef>
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace wfc {

using Id = std::size_t;

template<typename T>
using IdMap = std::unordered_map<Id, T>;

class IdSet {
public:
    class iterator {
    private:
        const std::vector<bool>& m_data;
        std::size_t m_current_index;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        iterator(const std::vector<bool>& data, std::size_t index);

        iterator& operator++();
        iterator operator++(int);
        value_type operator*() const;

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;

    private:
        void advance();
    };

private:
    std::vector<bool> m_data;
    std::size_t m_size;

public:
    IdSet();
    IdSet(Id max_id);

    void insert(const Id& id);
    void remove(const Id& id);
    void clear();

    bool contains(const Id& id) const;

    std::size_t capacity() const;
    std::size_t size() const;

    IdSet::iterator begin() const;
    IdSet::iterator end() const;
};

} // namespace wfc
