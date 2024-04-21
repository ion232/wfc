#pragma once

#include <initializer_list>
#include <vector>

namespace wfc::data {

class BitSet {
public:
    using Index = std::size_t;
    using Storage = std::uint64_t;

    static constexpr auto bits_in_byte = 8;
    static constexpr auto bits_in_storage = std::size_t(sizeof(std::uint64_t) * bits_in_byte);

    class iterator {
    private:
        const std::vector<Storage>& m_bits;
        Index m_current_index;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Index;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        iterator(
            const std::vector<Storage>& bits,
            const Index& index
        );

        iterator& operator++();
        iterator operator++(int);
        value_type operator*() const;

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;

    private:
        void advance();
    };

private:
    std::vector<Storage> m_bits;
    std::size_t m_size;

public:
    BitSet();
    BitSet(std::size_t count, bool value);

    void set(const Index& index);
    void unset(const Index& index);
    void intersect_with(const BitSet& bit_set);
    void union_with(const BitSet& bit_set);
    void clear();

    bool test(const Index& index) const noexcept;

    std::size_t capacity() const noexcept;
    std::size_t size() const noexcept;

    bool operator==(const BitSet& other) const noexcept;

    BitSet::iterator begin() const noexcept;
    BitSet::iterator end() const noexcept;
};

} // namespace wfc::data
