#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace wfc::data {

template<typename T>
class Tensor {
private:
    std::vector<std::size_t> m_dimensions;
    std::vector<T> m_data;

public:
    Tensor() = delete;
    Tensor(std::vector<std::size_t>&& dimensions, std::vector<T>&& data);
    Tensor(std::vector<std::size_t>&& dimensions, T value);

    T& operator[](std::size_t index);
    const T& operator[](std::size_t index) const;
    std::vector<std::optional<std::size_t>> adjacent(std::size_t index);
    std::vector<std::size_t> dimensions();
    std::size_t size() const;

    bool operator==(const Tensor<T>& other) const;

    friend class std::hash<Tensor<std::uint32_t>>;

private:
    std::vector<int64_t> index_to_coordinate(std::size_t index);
    std::size_t coordinate_to_index(std::vector<int64_t>&& coordinates);
};

} // namespace wfc::data

template<>
struct std::hash<wfc::data::Tensor<std::uint32_t>> {
    std::size_t operator()(const wfc::data::Tensor<std::uint32_t>& tensor) const noexcept;
};
