#pragma once

#include <cstdint>

namespace wfc::image {

using Byte = std::uint8_t;

struct Pixel {
    Byte r;
    Byte g;
    Byte b;
    Byte a;
};

Pixel make_pixel(Byte r, Byte g, Byte b, Byte a);
Pixel make_pixel(std::uint32_t pixel);

std::uint32_t rgba(const Pixel& pixel);
std::uint32_t argb(const Pixel& pixel);

} // namespace wfc::image
