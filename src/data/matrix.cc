#include "wfc/domain.h"
#include "wfc/data/matrix.h"

#include <numeric>

namespace wfc::data {

template<typename T>
Matrix<T>::Matrix(std::vector<std::size_t>&& dimensions, T value)
    : m_dimensions(std::move(dimensions))
    , m_data(std::reduce(m_dimensions.begin(), m_dimensions.end()), value)
{}

template<typename T>
T& Matrix<T>::operator[](std::size_t index) const {
    return m_data[index];
}

template<typename T>
std::vector<std::size_t> Matrix<T>::adjacent(std::size_t index) {
    auto adjacent_indices = std::vector<std::size_t>();
    auto coordinate = index_to_coordinate(index);

    for (std::size_t i = 0; i < coordinate.size(); i++) {
        for (std::ptrdiff_t offset : {-1, 1}) {
            if (coordinate[i] == 0 && offset == -1) {
                continue;
            }
            auto adjacent_coordinate = coordinate;
            adjacent_coordinate[i] = (adjacent_coordinate[i] + offset) % m_dimensions[i];
            auto adjacent_index = coordinate_to_index(adjacent_coordinate);
            adjacent_indices.emplace_back(std::move(adjacent_index));
        }
    }

    return adjacent_indices;
}

template<typename T>
std::vector<std::size_t> Matrix<T>::dimensions() {
    return m_dimensions;
}

template<typename T>
std::size_t Matrix<T>::size() const {
    return m_data.size();
}

template<typename T>
std::vector<size_t> Matrix<T>::index_to_coordinate(std::size_t index) {
    auto coordinates = std::vector<std::size_t>();

    for (auto it = m_dimensions.rbegin(); it != m_dimensions.rend(); it++) {
        auto component = std::size_t(index % *it);
        coordinates.push_back(component);
        index /= *it;
    }

    std::reverse(coordinates.begin(), coordinates.end());

    return coordinates;
}

template<typename T>
std::size_t Matrix<T>::coordinate_to_index(std::vector<size_t> coordinates) {
    auto index = std::size_t(0);
    auto accumulator = std::size_t(1);
    
    for (std::size_t i = m_dimensions.size() - 1; i < m_dimensions.size(); i--) {
        index += coordinates[i] * accumulator;
        accumulator *= m_dimensions[i];
    }
    
    return index;
}

template class Matrix<Domain>;

} // namespace wfc::data
