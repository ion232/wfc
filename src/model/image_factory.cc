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
        const auto full_area_size = static_cast<std::size_t>(std::reduce(
            pattern_dimensions.begin(),
            pattern_dimensions.end(),
            1,
            std::multiplies<>()
        ));
        auto id = std::size_t(0);
        auto weights_map = IdMap<std::size_t>();
        auto patterns_map = std::unordered_map<overlap::Pattern, Id>();

        for (std::size_t i = 0; i < image_tensor.size(); i++) {
            auto area = image_tensor.area_at(pattern_dimensions, i);

            if (area.size() != full_area_size) {
                continue;
            }

            auto pattern = overlap::Pattern(std::move(area));

            if (!patterns_map.contains(pattern)) {
                patterns_map.insert({pattern, id});
                weights_map.insert({id, 1});
                id++;
            }
        }

        auto tuple = std::make_tuple(id, patterns_map, weights_map);
        return tuple;
    }();

    auto pixels = [&patterns](){
        auto result = IdMap<image::Pixel>();

        for (const auto& [pattern, id] : patterns) {
            result.insert({id, pattern.value()});
        }

        return result;
    }();

    auto [constraints, supports] = [&image_tensor, &patterns](){
        auto constraint_offsets = image_tensor.offsets().size();
        auto constraints = std::vector<Image::Constraints>();
        auto supports = IdMap<std::size_t>();

        for (auto& [pattern, id] : patterns) {
            std::ignore = pattern;
            constraints.emplace_back(constraint_offsets, IdMap<std::size_t>());
            supports.insert({id, 1});
        }

        auto tuple = std::make_tuple(constraints, supports);
        return tuple;
    }();

    for (auto& [p1, id1] : patterns) {
        auto& constraints1 = constraints[id1];

        for (auto& [p2, id2] : patterns) { 
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
        std::move(pixels),
        std::move(supports)
    );

    return image;
}

} // namespace wfc::model
