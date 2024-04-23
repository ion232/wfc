#include "wfc/data/bit_set.h"

#include <bit>
#include <limits>

namespace wfc::data {
namespace {

inline BitSet::Index make_storage_index(const BitSet::Index& index) {
    return index / BitSet::bits_in_storage;
}

inline std::size_t make_storage_offset(const BitSet::Index& index) {
    return index % BitSet::bits_in_storage;
}

inline std::size_t make_bit_count(const std::vector<BitSet::Storage>& bits) {
    return bits.size() * BitSet::bits_in_storage;
}

inline bool test_index(const std::vector<BitSet::Storage>& bits, const BitSet::Index& index) {
    const auto storage_index = make_storage_index(index);
    const auto storage_offset = make_storage_offset(index);
    const auto is_set = bits[storage_index] & (1ull << storage_offset);
    return is_set;
}

} // namespace

BitSet::iterator::iterator(
    const std::vector<Storage>& bits,
    const Index& index
)
    : m_bits(bits)
    , m_current_index(index)
{
    const auto bit_count = make_bit_count(m_bits);

    if (m_current_index >= bit_count) {
        return;
    }

    if (test_index(m_bits, m_current_index)) {
        return;
    }

    advance();
}

BitSet::iterator& BitSet::iterator::operator++() {
    advance();
    return *this;
}

BitSet::iterator BitSet::iterator::operator++(int) {
    auto it = *this;
    advance();
    return it;
}

BitSet::iterator::value_type BitSet::iterator::operator*() const {
    return m_current_index;
}

bool BitSet::iterator::operator==(const BitSet::iterator& other) const {
    const auto same_index = (m_current_index == other.m_current_index);
    return same_index;
}

bool BitSet::iterator::operator!=(const BitSet::iterator& other) const {
    return !(*this == other);
}

void BitSet::iterator::advance() {
    const auto bit_count = make_bit_count(m_bits);

    if (m_current_index >= bit_count) {
        return;
    }

    m_current_index++;

    while (m_current_index < bit_count && !test_index(m_bits, m_current_index)) {
        m_current_index++;
    }
}

BitSet::BitSet()
    : m_bits()
    , m_size(0)
{}

BitSet::BitSet(std::size_t count, bool value)
    : m_bits()
    , m_size(count * static_cast<std::size_t>(value))
{
    if (count == 0) {
        return;
    }
    
    static constexpr auto all_bits_set = std::numeric_limits<Storage>::max();
    
    auto default_value = all_bits_set;
    // ion232: Overflow to get zero if value is false.
    default_value += static_cast<Storage>(!value);
    
    const auto storage_index = make_storage_index(count - 1);
    const auto storage_offset = make_storage_offset(count - 1);
    const auto mask = (all_bits_set >> (bits_in_storage - storage_offset - 1));

    m_bits = std::vector<Storage>(storage_index + 1, default_value);
    m_bits[m_bits.size() - 1] &= mask;
}

void BitSet::set(const Index& index) {
    const auto storage_index = make_storage_index(index);
    const auto storage_offset = make_storage_offset(index);

    auto& storage = m_bits[storage_index];
    const auto old_storage = m_bits[storage_index];
    const auto mask = 1ull << storage_offset;

    storage |= mask;
    m_size += static_cast<std::size_t>((old_storage ^ storage) >> storage_offset);
}

void BitSet::unset(const Index& index) {
    const auto storage_index = make_storage_index(index);
    const auto storage_offset = make_storage_offset(index);

    auto& storage = m_bits[storage_index];
    const auto old_storage = m_bits[storage_index];
    const auto mask = 1ull << storage_offset;

    storage &= (~mask);
    m_size -= static_cast<std::size_t>((old_storage ^ storage) >> storage_offset);
}

void BitSet::intersect_with(const BitSet& bit_set) {
    for (Index index = 0; index < m_bits.size(); index++) {
        const auto old_storage = m_bits[index];
        m_bits[index] &= bit_set.m_bits[index];
        const auto count = std::popcount(m_bits[index] ^ old_storage);
        m_size -= static_cast<std::size_t>(count);
    }
}

void BitSet::union_with(const BitSet& bit_set) {
    for (Index index = 0; index < m_bits.size(); index++) {
        const auto old_storage = m_bits[index];
        m_bits[index] |= bit_set.m_bits[index];
        const auto count = std::popcount(m_bits[index] ^ old_storage);
        m_size += static_cast<std::size_t>(count);
    }
}

bool BitSet::test(const Index& index) const noexcept {
    return test_index(m_bits, index);
}

void BitSet::clear() {
    for (auto& bytes : m_bits) {
        bytes ^= bytes;
    }

    m_size = 0;
}

std::size_t BitSet::capacity() const noexcept {
    return make_bit_count(m_bits);
}

std::size_t BitSet::size() const noexcept {
    return m_size;
}

bool BitSet::operator==(const BitSet& other) const noexcept {
    return m_bits == other.m_bits;
}

BitSet::iterator BitSet::begin() const noexcept {
    return BitSet::iterator(m_bits, 0);
}

BitSet::iterator BitSet::end() const noexcept {
    return BitSet::iterator(m_bits, make_bit_count(m_bits));
}

} // namespace wfc::data
