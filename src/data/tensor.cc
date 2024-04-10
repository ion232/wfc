#include "wfc/variable.h"
#include "wfc/data/tensor.h"

#include <iostream>
#include <numeric>

namespace wfc::data {

template<typename T>
Tensor<T>::Tensor(std::vector<std::size_t>&& dimensions, std::vector<T>&& data)
    : m_dimensions(std::move(dimensions))
    , m_data(std::move(data))
{}

template<typename T>
Tensor<T>::Tensor(std::vector<std::size_t>&& dimensions, T value)
    : m_dimensions(std::move(dimensions))
    , m_data(std::reduce(m_dimensions.begin(), m_dimensions.end(), 1ull, std::multiplies<>()), value)
{}

template<typename T>
T& Tensor<T>::operator[](std::size_t index) {
    return m_data[index];
}

template<typename T>
const T& Tensor<T>::operator[](std::size_t index) const {
    return m_data[index];
}

// TODO: Make sure this automatically corresponds to the Pattern adjacencies. Deduplicate this knowledge.
template<typename T>
std::vector<std::optional<std::size_t>> Tensor<T>::adjacent(std::size_t index) {
    auto coordinate = index_to_coordinate(index);
    
    const auto offsets = std::vector<std::pair<std::int64_t, std::int64_t>>({
        {-1, 0},
        {-1, -1},
        {0, -1},
        {1, -1},
        {1, 0},
        {1, 1},
        {0, 1},
        {-1, 1}
    });

    auto adj_coords = std::vector<std::optional<std::vector<std::int64_t>>>();
    for (auto& offset : offsets) {
        auto c = coordinate;
        const auto oob_min1 = (c[0] == 0 && offset.second == -1);
        const auto oob_max1 = (c[0] == (static_cast<int64_t>(m_dimensions[0]) - 1) && offset.second == 1);
        const auto oob_min2 = (c[1] == 0 && offset.first == -1);
        const auto oob_max2 = (c[1] == (static_cast<int64_t>(m_dimensions[1]) - 1) && offset.first == 1);

        if (oob_min1 || oob_max1 || oob_min2 || oob_max2) {
            adj_coords.emplace_back(std::nullopt);
            continue;
        }
        
        c[0] += offset.second;
        c[1] += offset.first;
        adj_coords.emplace_back(std::move(c));
    }
    
    auto adjacent_indices = std::vector<std::optional<std::size_t>>();
    for (auto& coord : adj_coords) {
        if (!coord) {
            adjacent_indices.emplace_back(std::nullopt);
            continue;
        }
        auto adjacent_index = static_cast<std::size_t>(coordinate_to_index(std::move(*coord)));
        adjacent_indices.emplace_back(std::move(adjacent_index));
    }

    return adjacent_indices;
}

template<typename T>
std::vector<std::size_t> Tensor<T>::dimensions() {
    return m_dimensions;
}

template<typename T>
std::size_t Tensor<T>::size() const {
    return m_data.size();
}

template<typename T>
bool Tensor<T>::operator==(const Tensor<T>& other) const {
    return this->m_data == other.m_data;
}

template<typename T>
std::vector<int64_t> Tensor<T>::index_to_coordinate(std::size_t index) {
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
std::size_t Tensor<T>::coordinate_to_index(std::vector<int64_t>&& coordinates) {
    auto index = std::size_t(0);
    auto accumulator = std::int64_t(1);
    
    for (std::size_t i = m_dimensions.size(); i > 0; i--) {
        index += coordinates[i - 1] * accumulator;
        accumulator *= static_cast<int64_t>(m_dimensions[i - 1]);
    }
    
    return index;
}

template class Tensor<Variable>;
template class Tensor<std::uint32_t>;

} // namespace wfc::data

std::size_t std::hash<wfc::data::Tensor<std::uint32_t>>::operator()(const wfc::data::Tensor<std::uint32_t>& tensor) const noexcept {
    auto h = std::hash<std::uint32_t>();
    auto result = std::size_t(1337);
    for (const auto& t : tensor.m_data) {
        result = result ^ (h(t) << 1ull);
    }
    return result;
}
