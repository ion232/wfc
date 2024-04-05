#pragma once

#include <array>
#include <vector>

namespace wfc::model::overlap {

class Side {
public:
    enum class Direction: uint8_t {
        Left,
        Right,
        Up,
        Down,
        Forward,
        Back,
        Unknown
    };

private:
    std::vector<std::uint32_t> m_elements;

public:
    Side(std::initializer_list<std::uint32_t> elements);
    Side(std::vector<std::uint32_t> elements);

    bool operator==(const Side& other) const;

    friend class std::hash<Side>;
};

} // namespace wfc::model::overlap

template<>
struct std::hash<wfc::model::overlap::Side> {
    std::size_t operator()(const wfc::model::overlap::Side& side) const noexcept;
};
