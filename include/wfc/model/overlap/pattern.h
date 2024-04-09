#pragma once

#include "wfc/data/tensor.h"

#include <cstdint>
#include <vector>

namespace wfc::model::overlap {

class Pattern {
private:
    data::Tensor<std::uint32_t> m_tensor;
    std::uint32_t m_value;

public:
    Pattern(data::Tensor<std::uint32_t>&& tensor, std::uint32_t value);

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
