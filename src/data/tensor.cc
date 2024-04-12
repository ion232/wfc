#include "wfc/variable.h"
#include "wfc/data/tensor.h"

#include <array>
#include <iostream>
#include <numeric>

namespace wfc::data {
namespace {
    using Range = std::pair<std::int32_t, std::int32_t>;

    std::vector<Degree> make_degrees(const std::vector<Range>& ranges, bool exclude_zero = true) {
        auto result = [&ranges](){
            const auto range = *ranges.cbegin();
            auto degrees = std::vector<Degree>();

            for (std::int32_t i = range.first; i <= range.second; i++) {
                degrees.emplace_back(Degree({i}));
            }

            return degrees;
        }();

        for (std::size_t i = 1; i < ranges.size(); i++) {
            auto next = std::vector<Degree>();
            auto range = ranges[i];

            for (std::int32_t c = range.first; c <= range.second; c++) {
                for (const auto& degree : result) {
                    auto extended_degree = degree;
                    extended_degree.emplace_back(c);
                    next.emplace_back(std::move(extended_degree));
                }
            }

            std::swap(next, result);
        }
        
        if (exclude_zero) {
            const auto is_zero = [&ranges](const auto& x){
                static const auto all_zeroes = Degree(ranges.size(), 0);
                return x == all_zeroes;
            };
            if (auto it = std::find_if(result.begin(), result.end(), is_zero); it != result.end()) {
                result.erase(it);
            }
        }

        return result;
    };
}

template<typename T>
Tensor<T>::Tensor(const std::vector<Dimension>& dimensions, std::vector<T>&& data)
    : m_dimensions(dimensions)
    , m_degrees(make_degrees(std::vector<Range>(dimensions.size(), Range{-1, 1})))
    , m_data(std::move(data))
{}

template<typename T>
Tensor<T>::Tensor(const std::vector<Dimension>& dimensions, T value)
    : m_dimensions(dimensions)
    , m_degrees(make_degrees(std::vector<Range>(dimensions.size(), Range{-1, 1})))
    , m_data(std::reduce(m_dimensions.begin(), m_dimensions.end(), 1, std::multiplies<>()), value)
{}

template<typename T>
T& Tensor<T>::operator[](Index index) {
    return m_data[index];
}

template<typename T>
const T& Tensor<T>::operator[](Index index) const {
    return m_data[index];
}

template<typename T>
Tensor<T> Tensor<T>::area_at(const std::vector<Dimension>& dimensions, Index least_index) const {
    auto degrees = [&dimensions](){
        auto ranges = std::vector<Range>();

        for (const auto& dimension : dimensions) {
            ranges.emplace_back(0, dimension - 1);
        }
        
        static constexpr auto exclude_zero = false;
        return make_degrees(std::move(ranges), exclude_zero);
    }();

    auto least_coordinate = index_to_coordinate(least_index);
    auto data = std::vector<T>();
    data.reserve(degrees.size());

    for (const auto& degree : degrees) {
        auto coordinate = least_coordinate;

        for (std::size_t i = 0; i < degree.size(); i++) {
            coordinate[i] += degree[i];
        }

        if (out_of_bounds(coordinate)) {
            continue;
        }

        auto index = coordinate_to_index(std::move(coordinate));
        auto t = m_data[index];
        data.emplace_back(std::move(t));
    }

    auto tensor = Tensor(dimensions, std::move(data));
    return tensor;
}

template<typename T>
Tensor<T> Tensor<T>::translated(const Degree& degree) const {
    auto data = std::vector<T>();
    data.reserve(m_data.size());

    for (Index index = 0; index < m_data.size(); index++) {
        auto coordinate = index_to_coordinate(index);

        for (std::size_t i = 0; i < coordinate.size(); i++) {
            coordinate[i] -= degree[i];
        }

        if (out_of_bounds(coordinate)) {
            continue;
        }
        
        auto t = m_data[index];
        data.emplace_back(std::move(t));
    }

    auto tensor = Tensor(m_dimensions, std::move(data));

    return tensor;
}

template<typename T>
std::vector<std::optional<Index>> Tensor<T>::indices_at_degrees_from(Index central_index) const {
    auto values = std::vector<std::optional<Index>>();
    auto central_coordinate = index_to_coordinate(central_index);

    for (const auto& degree : m_degrees) {
        auto coordinate = central_coordinate;

        for (std::size_t i = 0; i < degree.size(); i++) {
            coordinate[i] += degree[i];
        }

        if (out_of_bounds(coordinate)) {
            values.emplace_back(std::nullopt);
            continue;
        }

        auto index = coordinate_to_index(std::move(coordinate));
        values.emplace_back(std::move(index));
    }

    return values;
}

template<typename T>
const std::vector<Dimension>& Tensor<T>::dimensions() const {
    return m_dimensions;
}

template<typename T>
const std::vector<Degree>& Tensor<T>::degrees() const {
    return m_degrees;
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
Coordinate Tensor<T>::index_to_coordinate(Index index) const noexcept {
    auto coordinate = std::vector<std::int32_t>();

    for (std::size_t i = 0; i < m_dimensions.size(); i++) {
        const auto dimension = m_dimensions[i];
        auto component = std::int32_t(index % dimension);
        coordinate.push_back(component);
        index /= dimension;
    }

    return coordinate;
}

template<typename T>
Index Tensor<T>::coordinate_to_index(const Coordinate& coordinate) const noexcept {
    auto index = std::size_t(0);
    auto accumulator = std::int32_t(1);

    for (std::size_t i = 0; i < m_dimensions.size(); i++) {
        index += coordinate[i] * accumulator;
        accumulator *= static_cast<int32_t>(m_dimensions[i]);
    }

    return index;
}

template<typename T>
bool Tensor<T>::out_of_bounds(const Coordinate& coordinate) const noexcept {
    for (std::size_t i = 0; i < coordinate.size(); i++) {
        const auto is_too_small = coordinate[i] < 0;
        const auto is_too_large = coordinate[i] >= static_cast<std::int32_t>(m_dimensions[i]);
        if (is_too_small || is_too_large) {
            return true;
        }
    }

    return false;
}

template class Tensor<Variable>;
template class Tensor<image::Pixel>;

} // namespace wfc::data

std::size_t std::hash<wfc::data::Tensor<wfc::image::Pixel>>::operator()(const wfc::data::Tensor<wfc::image::Pixel>& tensor) const noexcept {
    auto h = std::hash<std::uint32_t>();
    auto result = std::size_t(1337);

    for (const auto& p : tensor.m_data) {
        const auto i = wfc::image::argb(p);
        result = result ^ (h(i) << 1ull);
    }

    return result;
}
