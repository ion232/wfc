#include "wfc/model/image_factory.h"
#include "wfc/model/overlap/pattern.h"

namespace wfc::model {

ImageFactory::ImageFactory(std::shared_ptr<image::interface::Loader> image_loader)
    : m_image_loader(std::move(image_loader))
{}

std::shared_ptr<Image> ImageFactory::make_image(
    const std::filesystem::path& path,
    const std::vector<data::Dimension>& pattern_dimensions
)
{
    auto image_tensor = [this, &path](){
        auto raw_pixels = m_image_loader->load_pixels(path);
        auto dimensions = std::vector<data::Dimension>({
            raw_pixels.begin()->size(),
            raw_pixels.size()
        });
        auto data = std::vector<image::Pixel>();
        data.reserve(raw_pixels.size() * raw_pixels.size());

        for (auto& row : raw_pixels) {
            std::move(row.begin(), row.end(), std::back_inserter(data));
        }

        auto tensor = data::Tensor<image::Pixel>(dimensions, std::move(data));
        return tensor;
    }();

    auto [max_id, patterns, weights] = [&pattern_dimensions, &image_tensor](){
        static constexpr auto pad = static_cast<bool>(false);

        auto id = std::size_t(0);
        auto weights_map = IdMap<std::size_t>();
        auto pattern_to_id = std::unordered_map<overlap::Pattern, Id>();
        auto patterns_map = IdMap<overlap::Pattern>();

        for (std::size_t i = 0; i < image_tensor.size(); i++) {
            auto area = image_tensor.area_at(pattern_dimensions, i, pad);
            auto pattern = overlap::Pattern(std::move(area));

            if (!pattern_to_id.contains(pattern)) {
                patterns_map.insert({id, pattern});
                weights_map.insert({id, 1});
                pattern_to_id.insert({pattern, id});
            } else {
                weights_map[pattern_to_id[pattern]]++;
                continue;
            }

            id++;
        }

        auto tuple = std::make_tuple(id, patterns_map, weights_map);
        return tuple;
    }();

    auto [constraints, supports] = [&image_tensor, &patterns](){
        auto constraint_offsets = image_tensor.offsets().size();
        auto constraints = std::vector<Image::Constraints>();
        auto supports = IdMap<std::size_t>();

        for (auto& [id, pattern] : patterns) {
            std::ignore = pattern;
            constraints.emplace_back(constraint_offsets, IdMap<std::size_t>());
            supports.insert({id, 1});
        }

        auto tuple = std::make_tuple(constraints, supports);
        return tuple;
    }();

    for (auto& [id1, p1] : patterns) {
        auto& constraints1 = constraints[id1];

        for (auto& [id2, p2] : patterns) { 
            auto overlaps = p1.overlaps(p2);

            for (std::size_t i = 0; i < overlaps.size(); i++) {
                if (!overlaps[i]) {
                    continue;
                }

                if (!constraints1[i].contains(id2)) {
                    constraints1[i][id2] = 1;
                } else {
                    constraints1[i][id2] += 1;
                }
            }
        }
    }

    auto image = std::make_shared<Image>(
        std::move(constraints),
        std::move(weights),
        std::move(patterns),
        std::move(supports)
    );

    return image;
}

} // namespace wfc::model
