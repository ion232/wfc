#include "wfc/image/pixel.h"

namespace wfc::image {
namespace {
    std::uint32_t to_uint32(Byte b1, Byte b2, Byte b3, Byte b4) {
        auto value = static_cast<std::uint32_t>(b1) << 24;
        value |= static_cast<std::uint32_t>(b2) << 16;
        value |= static_cast<std::uint32_t>(b3) << 8;
        value |= static_cast<std::uint32_t>(b4);
        return value;
    }
}

bool operator==(const Pixel& l, const Pixel& r) {
    return (l.r == r.r) && (l.g == r.g) && (l.b == r.b) && (l.a == r.a);
}

Pixel make_pixel(Byte r, Byte g, Byte b, Byte a) {
    return Pixel{r, g, b, a};
}

Pixel make_pixel(std::uint32_t pixel) {
    auto a = static_cast<std::uint8_t>((pixel >> 24) & 0xFF);
    auto r = static_cast<std::uint8_t>((pixel >> 16) & 0xFF);
    auto g = static_cast<std::uint8_t>((pixel >> 8) & 0xFF);
    auto b = static_cast<std::uint8_t>(pixel & 0xFF);
    return make_pixel(r, g, b, a);
}

std::uint32_t rgba(const Pixel& pixel) {
    return to_uint32(pixel.r, pixel.g, pixel.b, pixel.a);
}

std::uint32_t argb(const Pixel& pixel) {
    return to_uint32(pixel.a, pixel.r, pixel.g, pixel.b);
}

} // namespace wfc::image
