#include "wfc/image/pixel.h"

namespace wfc::image {

Pixel make_pixel(std::uint32_t pixel) {
    auto a = static_cast<std::uint8_t>((pixel >> 24) & 0xFF);
    auto r = static_cast<std::uint8_t>((pixel >> 16) & 0xFF);
    auto g = static_cast<std::uint8_t>((pixel >> 8) & 0xFF);
    auto b = static_cast<std::uint8_t>(pixel & 0xFF);
    return Pixel{ a, r, g, b };
}

std::uint32_t make_pixel(const Pixel& pixel) {
    auto value = static_cast<std::uint32_t>(pixel.a) << 24;
    value |= static_cast<std::uint32_t>(pixel.r) << 16;
    value |= static_cast<std::uint32_t>(pixel.g) << 8;
    value |= static_cast<std::uint32_t>(pixel.b);
    return value;
}

} // namespace wfc::image
