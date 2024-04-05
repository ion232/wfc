#include "wfc/model/image.h"
#include "wfc/model/overlap/pattern.h"

#include <map>
#include <set>
#include <tuple>

#include "lodepng/lodepng.h"

namespace wfc::model {
namespace {
    std::vector<std::vector<std::uint32_t>> load_image_pixels(std::filesystem::path path) {
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

        auto pixels = std::vector<std::vector<std::uint32_t>>(height, std::vector<std::uint32_t>(width, 0));

        // TODO: ion232: Clean this up a bit.
        for (std::size_t y = 0; y < height; y++) {
            for (std::size_t x = 0; x < width; x++) {
                const auto raw_index = (y * width + x) * sizeof(std::uint32_t);

                const auto r = static_cast<std::uint32_t>(raw_data[raw_index]);
                const auto g = static_cast<std::uint32_t>(raw_data[raw_index + 1]);
                const auto b = static_cast<std::uint32_t>(raw_data[raw_index + 2]);
                const auto a = static_cast<std::uint32_t>(raw_data[raw_index + 3]);

                pixels[y][x] = (r << 24) | (g << 16) | (b << 8) | a;
            }
        }

        return pixels;
    }
}

Image::Image(
    IdMap<Model::Adjacencies> adjacencies,
    Model::Weights weights,
    IdMap<std::uint32_t> pixels
)
    : m_adjacencies(std::move(adjacencies))
    , m_weights(std::move(weights))
    , m_pixels(std::move(pixels))
{}

Image::Adjacencies Image::lookup(Id id) {
    return m_adjacencies[id];
}

std::size_t Image::adjacent_count() {
    return m_adjacencies.begin()->second.size();
}

Image::Weights Image::weights() {
    return m_weights;
}

std::size_t Image::max_id() {
    if (m_pixels.empty()) {
        throw std::runtime_error("Empty pixels.");
    }

    return m_pixels.size() - 1;
}

std::vector<std::uint32_t> Image::make_pixels(const data::Matrix<Domain>& variables) {
    auto pixels = std::vector<std::uint32_t>();

    for (std::size_t index = 0; index < variables.size(); index++) {
        auto id = *variables[index].ids().begin();
        auto pixel = m_pixels[id];
        pixels.emplace_back(std::move(pixel));
    }

    return pixels;
}

std::optional<Image> load_image(std::filesystem::path path) {
    // TODO: ion232: Rename everything to match where it ends up. E.g. weights_map -> weights.
    // TODO: ion232: Sort out code relating to adjacency count, etc.
    auto pixels = load_image_pixels(std::move(path));

    constexpr auto make_adjacent = [](const auto& pixels, const auto x, const auto y) -> std::vector<std::uint32_t> {
        return {
            pixels[y - 1][x - 1],
            pixels[y - 1][x],
            pixels[y - 1][x + 1],
            pixels[y][x - 1],
            pixels[y][x + 1],
            pixels[y + 1][x - 1],
            pixels[y + 1][x],
            pixels[y + 1][x + 1],
        };
    };

    // TODO: ion232: Sort this magic out.
    constexpr auto make_sides = [](const auto& points) -> std::vector<overlap::Side> {
        return {
            {{points[0], points[3], points[5]}},
            {{points[2], points[4], points[7]}},
            {{points[0], points[1], points[2]}},
            {{points[5], points[6], points[7]}}
        };
    };

    auto unique_patterns = std::unordered_set<overlap::Pattern>();
    auto weights = std::unordered_map<overlap::Pattern, std::size_t>();

    for (std::size_t y = 1; y < pixels.size() - 1; y++) {
        for (std::size_t x = 1; x < pixels[y].size() - 1; x++) {
            auto points = make_adjacent(pixels, x, y);
            auto sides = make_sides(points);
            auto value = pixels[y][x];
            auto pattern = overlap::Pattern(value, std::move(sides));
            if (!weights.contains(pattern)) {
                weights.insert({pattern, 0});
            }
            weights[pattern] += 1;
            unique_patterns.emplace(std::move(pattern));
        }
    }

    auto id = std::size_t(0);
    auto pattern_map = IdMap<overlap::Pattern>();
    auto pattern_to_id = std::unordered_map<overlap::Pattern, Id>();
    for (auto& pattern : unique_patterns) {
        pattern_to_id.insert({pattern, id});
        pattern_map.insert({id, std::move(pattern)});
        id++;
    }

    auto pixel_map = IdMap<std::uint32_t>();
    for (const auto& [id, pattern] : pattern_map) {
        pixel_map.insert({id, pattern.value()});
    }

    auto weights_map = IdMap<std::size_t>();
    for (const auto& [pattern, count] : weights) {
        auto id = pattern_to_id[pattern];
        weights_map.insert({id, count});
    }

    static constexpr auto sides_count = 4;
    auto adjacency_map = IdMap<Image::Adjacencies>();
    for (auto& [id, _] : pattern_map) {
        adjacency_map.insert({id, Image::Adjacencies(sides_count, IdSet())});
    }

    for (auto& [p1, id1] : pattern_to_id) {
        auto& adjacencies = adjacency_map[id1];
        for (auto& [p2, id2] : pattern_to_id) {
            if (p1 == p2) {
                continue;
            }
            auto valid_adjacencies = p1.adjacencies(p2);
            for (std::size_t i = 0; i < valid_adjacencies.size(); i++) {
                const auto is_valid = valid_adjacencies[i];
                if (is_valid) {
                    adjacencies[i].insert(id2);
                }
            }
        }
    }

    auto image = std::make_optional<Image>(
        std::move(adjacency_map),
        std::move(weights_map),
        std::move(pixel_map)
    );

    return image;
}

} // namespace wfc::model
