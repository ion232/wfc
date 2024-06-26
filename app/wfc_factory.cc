#include "png_loader.h"
#include "wfc_factory.h"

#include "wfc/model/image_factory.h"
#include "wfc/model/preconstrainer.h"
#include "wfc/heuristic/assignment/sample.h"
#include "wfc/heuristic/choice/entropy.h"

namespace app {

WfcFactory::WfcFactory(const std::filesystem::path& image_path)
    : m_image_path(image_path)
    , m_image_model(nullptr)
{
    m_image_model = [&image_path = m_image_path](){
        auto png_loader = std::make_shared<app::PngLoader>();
        auto image_factory = wfc::model::ImageFactory(png_loader);
        auto pattern_dimensions = std::vector<wfc::data::Dimension>({3, 3});
        auto result = image_factory.make_image(image_path, pattern_dimensions);
        return result;
    }();
}

wfc::Wfc WfcFactory::make_wfc(std::size_t width, std::size_t height) {
    auto image = m_image_model;

    auto config = [&image](){
        auto random = std::make_shared<wfc::io::Random>();
        auto weights = std::make_shared<wfc::heuristic::Weights>(image->weights());
        auto sample = std::make_shared<wfc::heuristic::assignment::Sample>(weights, random);
        auto entropy = std::make_shared<wfc::heuristic::choice::Entropy>(weights, random);
        auto result = wfc::Wfc::Config{sample, entropy, image, random};
        return result;
    }();

    auto preconstraints = [&width, &height, &image_path = m_image_path](){
        auto result = wfc::model::Preconstrainer();

        if (image_path.filename() != "flowers.png") {
            return result;
        }

        static constexpr const auto has_dirt = [](const auto& pattern){
            static const auto dirt = wfc::image::make_pixel(185, 122, 87, 255);
            return pattern.contains(dirt);
        };

        for (std::int32_t x = 0; x < static_cast<std::int32_t>(width); x++) {
            for (std::int32_t y = 0; y < 2; y++) {
                const auto coordinate = wfc::data::Coordinate{x, static_cast<std::int32_t>(height) - y - 1};
                result.add(coordinate, has_dirt);
            }
        }

        return result;
    }();

    auto tensor = [&width, &height, &image, &preconstraints](){
        auto dimensions = std::vector<std::size_t>({width, height});
        auto variables = image->make_variables(dimensions);
        preconstraints.preconstrain(image->patterns(), variables);
        return variables;
    }();

    auto wfc = wfc::Wfc(std::move(config), std::move(tensor));

    return wfc;
}

std::filesystem::path WfcFactory::image_path() const noexcept {
    return m_image_path;
}

std::shared_ptr<wfc::model::Image> WfcFactory::image_model() const noexcept {
    return m_image_model;
}

} // namespace app
