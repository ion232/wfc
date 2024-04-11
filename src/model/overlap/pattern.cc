#include "wfc/model/overlap/pattern.h"

#include <array>

namespace wfc::model::overlap {

Pattern::Pattern(data::Tensor<std::uint32_t>&& tensor, std::uint32_t value)
    : m_tensor(std::move(tensor))
    , m_value(std::move(value))
{}

std::uint32_t Pattern::value() const {
    return m_value;
}

// TODO: ion232: Make this generic instead of 2D only.
// TODO: ion232: Remove the magic from this.
std::vector<bool> Pattern::adjacencies(const Pattern& other) const {
    static constexpr auto count = std::size_t(8);
    static const auto areas = std::array<std::vector<std::size_t>, count>({{
        {0, 1, 3, 4, 6, 7}, // l
        {0, 1, 3, 4}, // tl
        {0, 1, 2, 3, 4, 5}, // t
        {1, 2, 4, 5}, // tr
        {1, 2, 4, 5, 7, 8}, // r
        {4, 5, 7, 8}, // br
        {3, 4, 5, 6, 7, 8}, // b
        {3, 4, 6, 7} // bl
    }});

    auto adjacencies = std::vector<bool>(count, true);

    for (std::size_t i = 0; i < count; i++) {
        const auto& area = areas[i];
        const auto& other_area = areas[(i + 4) % count];

        for (std::size_t j = 0; j < area.size(); j++) {
            if (m_tensor[area[j]] != other.m_tensor[other_area[j]]) {
                adjacencies[i] = false;
                break;
            }
        }
    }

    return adjacencies;
}

bool Pattern::operator==(const Pattern& other) const {
    return this->m_tensor == other.m_tensor;
}

} // namespace wfc::model::overlap

std::size_t std::hash<wfc::model::overlap::Pattern>::operator()(const wfc::model::overlap::Pattern& pattern) const noexcept {
    auto h = std::hash<wfc::data::Tensor<std::uint32_t>>();
    auto result = std::size_t(1337);
    result = result ^ (h(pattern.m_tensor) << 1);
    return result;
}
