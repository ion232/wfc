#include "wfc/variable.h"
#include "wfc/data/tensor.h"

#include <array>
#include <iostream>
#include <numeric>

namespace wfc::data {
namespace {
    using Range = std::pair<std::int32_t, std::int32_t>;

    std::vector<Offset> make_offsets(const std::vector<Range>& ranges, bool exclude_zero = true) {
        auto result = [&ranges](){
            const auto range = *ranges.cbegin();
            auto offsets = std::vector<Offset>();

            for (std::int32_t i = range.first; i <= range.second; i++) {
                offsets.emplace_back(Offset({i}));
            }

            return offsets;
        }();

        for (std::size_t i = 1; i < ranges.size(); i++) {
            auto next = std::vector<Offset>();
            auto range = ranges[i];

            for (std::int32_t c = range.first; c <= range.second; c++) {
                for (const auto& offset : result) {
                    auto extended_offset = offset;
                    extended_offset.emplace_back(c);
                    next.emplace_back(std::move(extended_offset));
                }
            }

            std::swap(next, result);
        }
        
        if (exclude_zero) {
            const auto is_zero = [&ranges](const auto& x){
                static const auto all_zeroes = Offset(ranges.size(), 0);
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
    , m_offsets(make_offsets(std::vector<Range>(dimensions.size(), Range{-1, 1})))
    , m_data(std::move(data))
{}

template<typename T>
Tensor<T>::Tensor(const std::vector<Dimension>& dimensions, T value)
    : m_dimensions(dimensions)
    , m_offsets(make_offsets(std::vector<Range>(dimensions.size(), Range{-1, 1})))
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
Tensor<T> Tensor<T>::area_at(const std::vector<Dimension>& dimensions, Index least_index, bool pad) const {
    auto offsets = [&dimensions](){
        auto ranges = std::vector<Range>();

        for (const auto& dimension : dimensions) {
            ranges.emplace_back(0, dimension - 1);
        }
        
        static constexpr auto exclude_zero = false;
        return make_offsets(std::move(ranges), exclude_zero);
    }();

    auto least_coordinate = index_to_coordinate(least_index);
    auto data = std::vector<T>();
    data.reserve(offsets.size());

    for (const auto& offset : offsets) {
        auto coordinate = least_coordinate;

        for (std::size_t i = 0; i < offset.size(); i++) {
            coordinate[i] += offset[i];
        }

        if (pad) {
            coordinate = clamp(coordinate);
        } else if (out_of_bounds(coordinate)) {
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
Tensor<T> Tensor<T>::translated(const Offset& offset) const {
    auto data = std::vector<T>();
    data.reserve(m_data.size());

    for (Index index = 0; index < m_data.size(); index++) {
        auto coordinate = index_to_coordinate(index);

        for (std::size_t i = 0; i < coordinate.size(); i++) {
            coordinate[i] -= offset[i];
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
std::vector<std::optional<Index>> Tensor<T>::indices_at_offsets_from(Index central_index) const {
    auto values = std::vector<std::optional<Index>>();
    auto central_coordinate = index_to_coordinate(central_index);

    for (const auto& offset : m_offsets) {
        auto coordinate = central_coordinate;

        for (std::size_t i = 0; i < offset.size(); i++) {
            coordinate[i] += offset[i];
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
const std::vector<Offset>& Tensor<T>::offsets() const {
    return m_offsets;
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
Coordinate Tensor<T>::clamp(const Coordinate& coordinate) const noexcept {
    static constexpr auto low = std::int32_t(0);
    auto clamped_coordinate = Coordinate();

    for (std::size_t i = 0; i < coordinate.size(); i++) {
        const auto high = static_cast<std::int32_t>(m_dimensions[i] - 1);
        auto clamped_value = std::clamp(coordinate[i], low, high);
        clamped_coordinate.emplace_back(std::move(clamped_value));
    }

    return clamped_coordinate;
}

template<typename T>
Coordinate Tensor<T>::wrap(const Coordinate& coordinate) const noexcept {
    auto wrapped_coordinate = Coordinate();

    for (std::size_t i = 0; i < coordinate.size(); i++) {
        auto wrapped_value = coordinate[i] % m_dimensions[i];
        wrapped_coordinate.emplace_back(std::move(wrapped_value));
    }

    return wrapped_coordinate;
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
