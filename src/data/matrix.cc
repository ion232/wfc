#include "wfc/domain.h"
#include "wfc/data/matrix.h"

#include <iostream>
#include <numeric>

namespace wfc::data {

template<typename T>
Matrix<T>::Matrix(std::vector<std::size_t>&& dimensions, T value)
    : m_dimensions(std::move(dimensions))
    , m_data(std::reduce(m_dimensions.begin(), m_dimensions.end(), 1ull, std::multiplies<>()), value)
{}

template<typename T>
T& Matrix<T>::operator[](std::size_t index) {
    return m_data[index];
}

template<typename T>
const T& Matrix<T>::operator[](std::size_t index) const {
    return m_data[index];
}

template<typename T>
std::vector<std::optional<std::size_t>> Matrix<T>::adjacent(std::size_t index) {
    auto adjacent_indices = std::vector<std::optional<std::int64_t>>();
    auto coordinate = index_to_coordinate(index);

    for (std::size_t i = 0; i < coordinate.size(); i++) {
        for (std::int64_t offset : {-1, 1}) {
            const auto c = coordinate[i];
            const auto oob_min = (c == 0 && offset == -1);
            const auto oob_max = (c == (static_cast<int64_t>(m_dimensions[i]) - 1) && offset == 1);

            if (oob_min || oob_max) {
                adjacent_indices.emplace_back(std::nullopt);
                continue;
            }

            auto adjacent_coordinate = coordinate;
            adjacent_coordinate[i] = (adjacent_coordinate[i] + offset);
            auto adjacent_index = coordinate_to_index(std::move(adjacent_coordinate));
            adjacent_indices.emplace_back(std::move(adjacent_index));
        }
    }

    auto result = std::vector<std::optional<std::size_t>>();
    for (auto& i : adjacent_indices) {
        if (i && *i >= 0) {
            result.emplace_back(*i);
        } else {
            result.emplace_back(std::nullopt);
        }
    }

    return result;
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
std::vector<int64_t> Matrix<T>::index_to_coordinate(std::size_t index) {
    auto coordinates = std::vector<std::int64_t>();

    for (auto it = m_dimensions.rbegin(); it != m_dimensions.rend(); it++) {
        auto component = std::int64_t(index % *it);
        coordinates.push_back(component);
        index /= *it;
    }

    std::reverse(coordinates.begin(), coordinates.end());

    return coordinates;
}

template<typename T>
std::size_t Matrix<T>::coordinate_to_index(std::vector<int64_t>&& coordinates) {
    auto index = std::size_t(0);
    auto accumulator = std::int64_t(1);
    
    for (std::size_t i = m_dimensions.size() - 1; i > 0; i--) {
        index += coordinates[i] * accumulator;
        accumulator *= static_cast<int64_t>(m_dimensions[i]);
    }
    
    return index;
}

template class Matrix<Domain>;

} // namespace wfc::data
