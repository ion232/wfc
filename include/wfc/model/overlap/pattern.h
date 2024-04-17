#pragma once

#include "wfc/data/tensor.h"
#include "wfc/image/pixel.h"

#include <cstdint>
#include <vector>

namespace wfc::model::overlap {

class Pattern {
private:
    data::Tensor<image::Pixel> m_tensor;

public:
    Pattern(data::Tensor<image::Pixel>&& tensor);

    std::vector<bool> overlaps(const Pattern& other) const;
    bool contains(const image::Pixel& pixel) const;

    image::Pixel value() const;

    bool operator==(const Pattern& other) const;

    friend class std::hash<Pattern>;
};

} // namespace wfc::model::overlap

template<>
struct std::hash<wfc::model::overlap::Pattern> {
    std::size_t operator()(const wfc::model::overlap::Pattern& pattern) const noexcept;
};
