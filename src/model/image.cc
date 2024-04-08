#include "wfc/model/image.h"
#include "wfc/model/overlap/pattern.h"

#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <tuple>

#include "lodepng.h"

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

                pixels[y][x] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }

        return pixels;
    }
}

Image::Image(
    IdMap<Model::Adjacencies> adjacencies,
    Model::Weights weights,
    IdMap<std::uint32_t> pixels,
    IdMap<std::size_t> support_counts
)
    : m_adjacencies(std::move(adjacencies))
    , m_weights(std::move(weights))
    , m_pixels(std::move(pixels))
    , m_support_counts(std::move(support_counts))
{}

Image::Adjacencies Image::lookup(Id id) {
    return m_adjacencies[id];
}

std::size_t Image::adjacent_count() {
    return 8;
}

Image::Weights Image::weights() {
    return m_weights;
}

Domain Image::initial_domain() {
    auto map = m_support_counts;
    auto domain = Domain(std::move(map));
    return domain;
}

std::vector<std::uint32_t> Image::make_pixels(const data::Matrix<Domain>& variables) {
    auto pixels = std::vector<std::uint32_t>();

    for (std::size_t index = 0; index < variables.size(); index++) {
        auto ids = variables[index].ids();
        auto pixel = [this, &ids](){
            if (ids.size() == 1) {
                return m_pixels[*ids.begin()];
            } else if (ids.size() >= 2) {
                auto green = static_cast<uint8_t>((ids.size() * 255) / 64);

                // Construct the ARGB color value with full opacity (0xFF000000)
                // and the green component shifted to the appropriate position
                return std::uint32_t(0xFF000000 | (green << 8));
            } else {
                return std::numeric_limits<std::uint32_t>::max();
            }
        }();
        pixels.emplace_back(std::move(pixel));
    }

    return pixels;
}

std::optional<Image> load_image(std::filesystem::path path) {
    // TODO: ion232: Rename everything to match where it ends up. E.g. weights_map -> weights.
    // TODO: ion232: Sort out code relating to adjacency count, etc.
    auto pixels = load_image_pixels(std::move(path));

    constexpr auto make_block = [](const auto& pixels, const auto x, const auto y) -> std::vector<std::uint32_t> {
        return {
            pixels[y - 1][x - 1], // 0
            pixels[y - 1][x], // 1
            pixels[y - 1][x + 1], // 2
            pixels[y][x - 1], // 3
            pixels[y][x], // 4
            pixels[y][x + 1], // 5
            pixels[y + 1][x - 1], // 6
            pixels[y + 1][x], // 7
            pixels[y + 1][x + 1], // 8
        };
    };

    // TODO: ion232: Sort this magic out.
    // constexpr auto make_sides = [](const auto& block) -> std::vector<overlap::Side> {
    //     return {
    //         {{block[0], block[3], block[5]}},
    //         {{block[1], block[4], block[7]}},
    //         {{block[2], block[5], block[8]}},
    //         {{block[0], block[1], block[2]}},
    //         {{block[3], block[4], block[5]}},
    //         {{block[6], block[7], block[8]}},
    //     };
    // };

    auto unique_patterns = std::unordered_set<overlap::Pattern>();
    auto weights = std::unordered_map<overlap::Pattern, std::size_t>();

    for (std::size_t y = 1; y < pixels.size() - 1; y++) {
        for (std::size_t x = 1; x < pixels[y].size() - 1; x++) {
            auto block = make_block(pixels, x, y);
            // TODO: ion232: More magic to refactor.
            auto matrix = data::Matrix<std::uint32_t>({3, 3}, std::move(block));
            auto value = pixels[y][x];
            auto pattern = overlap::Pattern(std::move(matrix), value);
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

    // TODO: ion232: Make this derived or otherwise configurable.
    static constexpr auto adjacent_count = std::size_t(8);
    auto adjacency_map = IdMap<Image::Adjacencies>();
    auto support_map = IdMap<std::size_t>();
    for (auto& [id, _] : pattern_map) {
        adjacency_map.insert({id, Image::Adjacencies(adjacent_count, IdMap<std::size_t>())});
        support_map.insert({id, 1});
    }

    for (auto& [p1, id1] : pattern_to_id) {
        auto& adjacencies1 = adjacency_map[id1];
        for (auto& [p2, id2] : pattern_to_id) { 
            auto valid_adjacencies = p1.adjacencies(p2);
            for (std::size_t i = 0; i < valid_adjacencies.size(); i++) {
                const auto is_valid = valid_adjacencies[i];
                if (is_valid) {
//                    support_map[id1] += 1;
                    if (!adjacencies1[i].contains(id2)) {
                        adjacencies1[i][id2] = 1;
                    } else {
                        adjacencies1[i][id2] += 1;
                    }
                }
            }
            
        }
    }

    auto image = std::make_optional<Image>(
        std::move(adjacency_map),
        std::move(weights_map),
        std::move(pixel_map),
        std::move(support_map)
    );

    return image;
}

} // namespace wfc::model
