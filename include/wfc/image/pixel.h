#pragma once

#include <cstdint>

namespace wfc::image {

struct Pixel {
    std::uint8_t a;
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

Pixel make_pixel(std::uint32_t pixel);
std::uint32_t make_pixel(const Pixel& pixel);

} // namespace wfc::image
