#include "wfc/model/image.h"
#include "wfc/model/overlap/pattern.h"

#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <tuple>

namespace wfc::model {

Image::Image(
    std::vector<Constraints> constraints,
    Model::Weights weights,
    IdMap<image::Pixel> pixels,
    IdMap<std::size_t> support_counts
)
    : m_constraints(std::move(constraints))
    , m_weights(std::move(weights))
    , m_pixels(std::move(pixels))
    , m_support_counts(std::move(support_counts))
{}

Image::Constraints& Image::constraints(Id id) {
    return m_constraints[id];
}

std::size_t Image::constraint_degrees() const noexcept {
    return m_constraints.begin()->size();
}

Image::Weights Image::weights() {
    return m_weights;
}

Variable Image::make_variable() {
    auto supports = m_support_counts;
    auto variable = Variable(std::move(supports));
    return variable;
}

std::vector<image::Pixel> Image::make_pixels(const data::Tensor<Variable>& variables) {
    static const auto make_pixel = [this](const auto& variable){
        const auto state = variable.state();
        const auto ids = variable.ids();

        if (state == Variable::State::Decided) {
            const auto id = *ids.begin();
            const auto pixel = m_pixels[id];
            return pixel;
        } else if (state == Variable::State::Undecided) {
            static constexpr auto max_byte = std::numeric_limits<image::Byte>::max();

            const auto domain_size = ids.size();

            auto r = std::uint32_t(0);
            auto g = std::uint32_t(0);
            auto b = std::uint32_t(0);
            auto a = static_cast<image::Byte>((variable.size() * max_byte) / domain_size);

            for (const auto& id : ids) {
                const auto pixel = m_pixels[id];
                r += pixel.r;
                g += pixel.g;
                b += pixel.b;
            }

            r = static_cast<image::Byte>(r / domain_size);
            g = static_cast<image::Byte>(g / domain_size);
            b = static_cast<image::Byte>(b / domain_size);

            auto pixel = image::make_pixel(r, g, b, a);
            return pixel;
        } else {
            auto pixel = image::make_pixel(std::numeric_limits<std::uint32_t>::max());
            return pixel;
        }
    };

    auto pixels = std::vector<image::Pixel>();

    for (data::Index index = 0; index < variables.size(); index++) {
        const auto& variable = variables[index];
        auto pixel = make_pixel(variable);
        pixels.emplace_back(std::move(pixel));
    }

    return pixels;
}

} // namespace wfc::model
