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
    IdMap<overlap::Pattern> patterns
)
    : m_constraints(std::move(constraints))
    , m_weights(std::move(weights))
    , m_patterns(std::move(patterns))
{}

Image::Constraints& Image::constraints(Id id) {
    return m_constraints[id];
}

std::size_t Image::constraint_offsets() const noexcept {
    return m_constraints.begin()->size();
}

Image::Weights Image::weights() {
    return m_weights;
}

const Image::Patterns& Image::patterns() const noexcept {
    return m_patterns;
}

data::Tensor<Variable> Image::make_variables(const std::vector<data::Dimension>& dimensions) {
    auto id_set = IdSet();
    for (Id id = 0; id < m_patterns.size() - 1; id++) {
        id_set.insert(id);
    }
    auto variable = Variable(std::move(id_set));
    auto tensor = data::Tensor<Variable>(dimensions, std::move(variable));

    return tensor;
}

std::vector<image::Pixel> Image::make_pixels(
    const data::Tensor<Variable>& variables) {
    static const auto make_pixel = [this](const auto& variable){
        const auto state = variable.state();
        const auto ids = variable.ids();

        if (state == Variable::State::Decided) {
            const auto id = *ids.begin();
            const auto pixel = m_patterns.at(id).value();
            return pixel;
        } else if (state == Variable::State::Undecided) {
            static constexpr auto max_byte = std::numeric_limits<image::Byte>::max();

            const auto domain_size = ids.size();

            auto r = std::uint32_t(0);
            auto g = std::uint32_t(0);
            auto b = std::uint32_t(0);
            auto a = static_cast<image::Byte>((variable.size() * max_byte) / domain_size);

            for (const auto& id : ids) {
                const auto pixel = m_patterns.at(id).value();
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
