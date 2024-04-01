#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace wfc::data {

template<typename T>
class Matrix {
private:
    std::vector<std::size_t> m_dimensions;
    std::vector<T> m_data;

public:
    Matrix() = delete;
    Matrix(std::vector<std::size_t> dimensions, T value);

    T& operator[](std::size_t index);
    std::vector<std::size_t> adjacent(std::size_t index);
    std::vector<std::size_t> dimensions();
    std::size_t size();

private:
    std::vector<size_t> index_to_coordinate(std::size_t index);
    std::size_t coordinate_to_index(std::vector<size_t> coordinates);
};

} // namespace wfc::data
