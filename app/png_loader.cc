#include "png_loader.h"

#include "lodepng.h"

namespace wfc::image {

PngLoader::Pixels PngLoader::load_pixels(const std::filesystem::path& path) {
    static const auto throw_error = [](const auto& message, auto error){
        throw std::runtime_error(message + std::to_string(error));
    };

    auto png = std::vector<unsigned char>();
    auto raw_data = std::vector<unsigned char>();
    auto width = static_cast<unsigned int>(0);
    auto height = static_cast<unsigned int>(0);

    if (auto error = lodepng::load_file(png, path.string()); error != 0) {
        throw_error("Error loading png file: ", error);
    }

    if (auto error = lodepng::decode(raw_data, width, height, png); error != 0) {
        throw_error("Error decoding png image: ", error);
    }

    auto pixels = Pixels();
    pixels.reserve(height);

    for (std::size_t y = 0; y < height; y++) {
        auto row = std::vector<Pixel>();
        row.reserve(width);

        for (std::size_t x = 0; x < width; x++) {
            auto raw_index = (y * width + x) * sizeof(std::uint32_t);

            const auto r = static_cast<std::uint8_t>(raw_data[raw_index++]);
            const auto g = static_cast<std::uint8_t>(raw_data[raw_index++]);
            const auto b = static_cast<std::uint8_t>(raw_data[raw_index++]);
            const auto a = static_cast<std::uint8_t>(raw_data[raw_index++]);

            row.emplace_back(make_pixel(r, g, b, a));
        }

        pixels.emplace_back(std::move(row));
    }

    return pixels;
}

} // namespace wfc::image
