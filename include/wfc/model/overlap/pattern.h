#pragma once

#include "wfc/model/overlap/side.h"

#include <vector>

namespace wfc::model::overlap {

class Pattern {
private:
    std::uint32_t m_value;
    std::vector<Side> m_sides;

public:
    Pattern(std::uint32_t value, std::vector<Side>&& sides);

    std::uint32_t value() const;
    std::vector<bool> adjacencies(const Pattern& other) const;

    bool operator==(const Pattern& other) const;

    friend class std::hash<Pattern>;
};

} // namespace wfc::model::overlap

template<>
struct std::hash<wfc::model::overlap::Pattern> {
    std::size_t operator()(const wfc::model::overlap::Pattern& pattern) const noexcept;
};
