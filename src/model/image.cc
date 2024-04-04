#include "wfc/model/image.h"

#include <map>
#include <set>
#include <tuple>

#include "lodepng/lodepng.h"

namespace wfc::model {
namespace {
    std::vector<std::vector<uint32_t>> load_image_pixels(std::filesystem::path path) {
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

        auto pixels = std::vector<std::vector<uint32_t>>(height, std::vector<uint32_t>(width, 0));

        for (std::size_t y = 0; y < height; y++) {
            for (std::size_t x = 0; x < width; x++) {
                const auto raw_index = (y * width + x) * sizeof(uint32_t);

                const auto r = static_cast<uint32_t>(raw_data[raw_index]);
                const auto g = static_cast<uint32_t>(raw_data[raw_index + 1]);
                const auto b = static_cast<uint32_t>(raw_data[raw_index + 2]);
                const auto a = static_cast<uint32_t>(raw_data[raw_index + 3]);
                
                pixels[y][x] = (r << 24) | (g << 16) | (b << 8) | a;
            }
        }

        return pixels;
    }
}

Image::Image(
    IdMap<std::unordered_set<Id>> adjacencies,
    IdMap<std::size_t> weights,
    IdMap<uint32_t> pixels
)
    : m_adjacencies(std::move(adjacencies))
    , m_weights(std::move(weights))
    , m_pixels(std::move(pixels))
{}

std::unordered_set<Id> Image::lookup(Id id) {
    return m_adjacencies[id];
}

IdMap<std::size_t> Image::weights() {
    return std::unordered_map<Id, std::size_t>();
}

std::size_t Image::max_id() {
    if (m_pixels.empty()) {
        throw std::runtime_error("Empty pixels.");
    }

    return m_pixels.size() - 1;
}

std::vector<uint32_t> Image::make_pixels(const data::Matrix<Domain>& variables) {
    auto pixels = std::vector<uint32_t>();

    for (std::size_t index = 0; index < variables.size(); index++) {
        auto id = *variables[index].ids().begin();
        auto pixel = m_pixels[id];
        pixels.emplace_back(std::move(pixel));
    }

    return pixels;
}

std::optional<Image> load_image(std::filesystem::path path) {
    auto p = load_image_pixels(std::move(path));

    using Side = std::tuple<uint32_t, uint32_t, uint32_t>;
    using Pattern = std::set<Side>;
    using OverlapMap = std::map<Pattern, std::set<Pattern>>;

    auto rows = p.size();
    auto columns = p.begin()->size();
    auto patterns = std::set<Pattern>();

    for (int i = 0; i < rows - 2; i++) {
        for (int j = 0; j < columns - 2; j++) {
            Side left_column = {
                p[i][j],
                p[i + 1][j],
                p[i + 2][j]
            };
            Side top_row = {
                p[i][j],
                p[i][j + 1],
                p[i][j + 2]
            };
            Side right_column = {
                p[i][j + 2],
                p[i + 1][j + 2],
                p[i + 2][j + 2]
            };
            Side bottom_row = {
                p[i + 2][j],
                p[i + 2][j + 1],
                p[i + 2][j + 2]
            };

            auto pattern = Pattern();
            pattern.insert(left_column);
            pattern.insert(top_row);
            pattern.insert(right_column);
            pattern.insert(bottom_row);

            patterns.insert(std::move(pattern));
        }
    }

    auto overlaps = OverlapMap();

    for (const auto& current_pattern : patterns) {
        for (const auto& target_pattern : patterns) {
            if (current_pattern == target_pattern) {
                continue;
            }
        }
    }

    return {};
}

} // namespace wfc::model
