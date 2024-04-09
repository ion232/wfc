#include "png_loader.h"

#include "lodepng.h"

namespace wfc::image {

Pixels PngLoader::load_pixels(const std::filesystem::path& path) {
    auto png = std::vector<unsigned char>();
    auto raw_data = std::vector<unsigned char>();
    auto width = static_cast<unsigned int>(0);
    auto height = static_cast<unsigned int>(0);

    auto error = lodepng::load_file(png, path.string());
    if (error) {
        throw std::runtime_error("Error loading PNG file: " + std::to_string(error));
    }

    error = lodepng::decode(raw_data, width, height, png);
    if (error) {
        throw std::runtime_error("Error decoding PNG image: " + std::to_string(error));
    }

    auto pixels = Pixels(height, std::vector<Pixel>(width, {0, 0, 0, 0}));

    // TODO: ion232: Clean this up a bit.
    for (std::size_t y = 0; y < height; y++) {
        for (std::size_t x = 0; x < width; x++) {
            const auto raw_index = (y * width + x) * sizeof(std::uint32_t);

            const auto r = static_cast<std::uint32_t>(raw_data[raw_index]);
            const auto g = static_cast<std::uint32_t>(raw_data[raw_index + 1]);
            const auto b = static_cast<std::uint32_t>(raw_data[raw_index + 2]);
            const auto a = static_cast<std::uint32_t>(raw_data[raw_index + 3]);

            pixels[y][x] = make_pixel((a << 24) | (r << 16) | (g << 8) | b);
        }
    }

    return pixels;
}

} // namespace wfc::image
