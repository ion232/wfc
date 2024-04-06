#include "wfc/model/overlap/side.h"

namespace wfc::model::overlap {

Side::Side(std::initializer_list<std::uint32_t> elements)
    : m_elements(std::move(elements))
{}

Side::Side(std::vector<std::uint32_t> elements)
    : m_elements(std::move(elements))
{}

bool Side::operator==(const Side& other) const {
    auto equal = true;
    for (std::size_t i = 0; i < m_elements.size(); i++) {
        equal = equal && (this->m_elements[i] == other.m_elements[i]);
    }
    return equal;
}

} // namespace wfc::model::overlap

std::size_t std::hash<wfc::model::overlap::Side>::operator()(const wfc::model::overlap::Side& side) const noexcept {
    auto h = std::hash<std::uint32_t>();
    auto result = std::size_t(1337);
    for (const auto& x : side.m_elements) {
        result = result ^ (h(x) << 1ull);
    }
    return result;
}
