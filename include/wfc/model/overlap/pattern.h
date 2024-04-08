#pragma once

#include "wfc/data/matrix.h"

#include <cstdint>
#include <vector>

namespace wfc::model::overlap {

class Pattern {
private:
    data::Matrix<std::uint32_t> m_matrix;
    std::uint32_t m_value;

public:
    Pattern(data::Matrix<std::uint32_t>&& matrix, std::uint32_t value);

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
