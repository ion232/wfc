#include "wfc/model/overlap/pattern.h"

namespace wfc::model::overlap {

Pattern::Pattern(std::uint32_t value, std::vector<Side>&& sides)
    : m_value(std::move(value))
    , m_sides(std::move(sides))
{}

std::uint32_t Pattern::value() const {
    return m_value;
}

std::vector<bool> Pattern::adjacencies(const Pattern& other) const {
    static constexpr auto direction_count = static_cast<std::size_t>(Side::Direction::Unknown);
    static constexpr auto opposite_directions = std::array<Side::Direction, direction_count>{
        Side::Direction::Right,
        Side::Direction::Left,
        Side::Direction::Down,
        Side::Direction::Up,
        Side::Direction::Back,
        Side::Direction::Forward
    };
    auto adjacencies = std::vector<bool>();
    for (std::size_t i = 0; i < m_sides.size(); i++) {
        auto side = m_sides[i];
        auto opposite_index = static_cast<std::size_t>(opposite_directions[i]);
        auto opposite_side = other.m_sides[opposite_index];
        adjacencies.emplace_back((side == opposite_side));
    }
    return adjacencies;
}

bool Pattern::operator==(const Pattern& other) const {
    return this->m_sides == other.m_sides;
}

} // namespace wfc::model::overlap

std::size_t std::hash<wfc::model::overlap::Pattern>::operator()(const wfc::model::overlap::Pattern& pattern) const noexcept {
    auto h = std::hash<wfc::model::overlap::Side>();
    auto result = std::size_t(1337);
    for (const auto& side : pattern.m_sides) {
        result = result ^ (h(side) << 1ull);
    }
    return result;
}
