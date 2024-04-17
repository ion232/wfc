#include "wfc/model/overlap/pattern.h"

#include <array>

namespace wfc::model::overlap {

Pattern::Pattern(data::Tensor<image::Pixel>&& tensor)
    : m_tensor(std::move(tensor))
{}

std::vector<bool> Pattern::overlaps(const Pattern& other) const {
    // ion232: No need to cache this really, it's already fast enough.
    const auto make_areas = [](const auto& tensor){
        auto result = std::vector<data::Tensor<image::Pixel>>();

        for (const auto& offset : tensor.offsets()) {
            auto area = tensor.translated(offset);
            result.emplace_back(std::move(area));
        }

        return result;
    };

    const auto areas = make_areas(m_tensor);
    const auto other_areas = make_areas(other.m_tensor);
    const auto offset_count = m_tensor.offsets().size();
    auto overlaps = std::vector<bool>(offset_count, true);

    for (std::size_t i = 0; i < offset_count; i++) {
        const auto& area = areas[i];
        const auto& other_area = other_areas[other_areas.size() - i - 1];

        overlaps[i] = (area == other_area);
    }

    return overlaps;
}

bool Pattern::contains(const image::Pixel& pixel) const {
    for (std::size_t i = 0; i < m_tensor.size(); i++) {
        if (m_tensor[i] == pixel) {
            return true;
        }
    }

    return false;
}

image::Pixel Pattern::value() const {
    return m_tensor[0];
}

bool Pattern::operator==(const Pattern& other) const {
    return this->m_tensor == other.m_tensor;
}

} // namespace wfc::model::overlap

std::size_t std::hash<wfc::model::overlap::Pattern>::operator()(const wfc::model::overlap::Pattern& pattern) const noexcept {
    auto h = std::hash<wfc::data::Tensor<wfc::image::Pixel>>();
    auto result = std::size_t(1337);
    result = result ^ (h(pattern.m_tensor) << 1);
    return result;
}
