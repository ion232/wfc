#include "wfc/model/image_factory.h"
#include "wfc/model/overlap/pattern.h"

namespace wfc::model {

ImageFactory::ImageFactory(std::shared_ptr<image::interface::Loader> image_loader)
    : m_image_loader(std::move(image_loader))
{}

std::shared_ptr<Image> ImageFactory::make_image(const std::filesystem::path& path) {
    auto pixels = m_image_loader->load_pixels(path);

    // TODO: ion232: Rename everything to match where it ends up. E.g. weights_map -> weights.
    // TODO: ion232: Sort out code relating to adjacency count, etc.
    constexpr auto make_block = [](const auto& pixels, const auto x, const auto y) -> std::vector<std::uint32_t> {
        return {
            argb(pixels[y - 1][x - 1]), // 0
            argb(pixels[y - 1][x]), // 1
            argb(pixels[y - 1][x + 1]), // 2
            argb(pixels[y][x - 1]), // 3
            argb(pixels[y][x]), // 4
            argb(pixels[y][x + 1]), // 5
            argb(pixels[y + 1][x - 1]), // 6
            argb(pixels[y + 1][x]), // 7
            argb(pixels[y + 1][x + 1]), // 8
        };
    };

    auto unique_patterns = std::unordered_set<overlap::Pattern>();
    auto weights = std::unordered_map<overlap::Pattern, std::size_t>();

    for (std::size_t y = 1; y < pixels.size() - 1; y++) {
        for (std::size_t x = 1; x < pixels[y].size() - 1; x++) {
            auto block = make_block(pixels, x, y);
            // TODO: ion232: More magic to refactor.
            auto tensor = data::Tensor<std::uint32_t>({3, 3}, std::move(block));
            auto value = argb(pixels[y][x]);
            auto pattern = overlap::Pattern(std::move(tensor), value);
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
    auto adjacency_map = std::vector<Image::Adjacencies>();
    auto support_map = IdMap<std::size_t>();
    for (auto& [id, _] : pattern_map) {
        adjacency_map.emplace_back(adjacent_count, IdMap<std::size_t>());
        support_map.insert({id, 1});
    }

    for (auto& [p1, id1] : pattern_to_id) {
        auto& adjacencies1 = adjacency_map[id1];
        for (auto& [p2, id2] : pattern_to_id) { 
            auto valid_adjacencies = p1.adjacencies(p2);
            for (std::size_t i = 0; i < valid_adjacencies.size(); i++) {
                const auto is_valid = valid_adjacencies[i];
                if (is_valid) {
                    if (!adjacencies1[i].contains(id2)) {
                        adjacencies1[i][id2] = 1;
                    } else {
                        adjacencies1[i][id2] += 1;
                    }
                }
            }
            
        }
    }

    auto image = std::make_shared<Image>(
        std::move(adjacency_map),
        std::move(weights_map),
        std::move(pixel_map),
        std::move(support_map)
    );

    return image;
}

} // namespace wfc::model