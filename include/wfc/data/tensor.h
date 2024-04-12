#pragma once

#include "wfc/image/pixel.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace wfc::data {

using Dimension = std::size_t;
using Degree = std::vector<std::int32_t>;
using Coordinate = std::vector<std::int32_t>;
using Index = std::size_t;

template<typename T>
class Tensor {
private:
    std::vector<Dimension> m_dimensions;
    std::vector<Degree> m_degrees;
    std::vector<T> m_data;

public:
    Tensor() = delete;
    Tensor(const std::vector<Dimension>& dimensions, std::vector<T>&& data);
    Tensor(const std::vector<Dimension>& dimensions, T value);

    T& operator[](Index index);
    const T& operator[](Index index) const;

    Tensor<T> area_at(const std::vector<Dimension>& dimensions, Index least_index) const;
    Tensor<T> translated(const Degree& degree) const;
    std::vector<std::optional<Index>> indices_at_degrees_from(Index central_index) const;

    const std::vector<Dimension>& dimensions() const;
    const std::vector<Degree>& degrees() const;
    std::size_t size() const;

    bool operator==(const Tensor<T>& other) const;

    friend class std::hash<Tensor<image::Pixel>>;

private:
    Coordinate index_to_coordinate(Index index) const noexcept;
    Index coordinate_to_index(const Coordinate& coordinate) const noexcept;
    bool out_of_bounds(const Coordinate& coordinate) const noexcept;
};

} // namespace wfc::data

template<>
struct std::hash<wfc::data::Tensor<wfc::image::Pixel>> {
    std::size_t operator()(const wfc::data::Tensor<wfc::image::Pixel>& tensor) const noexcept;
};
