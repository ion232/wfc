#include "wfc/model/image.h"
#include "wfc/model/overlap/pattern.h"

#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <tuple>

namespace wfc::model {

Image::Image(
    std::vector<Model::Adjacencies> adjacencies,
    Model::Weights weights,
    IdMap<std::uint32_t> pixels,
    IdMap<std::size_t> support_counts
)
    : m_adjacencies(std::move(adjacencies))
    , m_weights(std::move(weights))
    , m_pixels(std::move(pixels))
    , m_support_counts(std::move(support_counts))
{}

Image::Adjacencies& Image::adjacencies(Id id) {
    return m_adjacencies[id];
}

std::size_t Image::adjacency_count() const noexcept {
    return m_adjacencies.begin()->size();
}

Image::Weights Image::weights() {
    return m_weights;
}

Variable Image::initial_variable() {
    auto map = m_support_counts;
    auto variable = Variable(std::move(map));
    return variable;
}

std::vector<std::uint32_t> Image::make_pixels(const data::Tensor<Variable>& variables) {
    auto pixels = std::vector<std::uint32_t>();

    for (std::size_t index = 0; index < variables.size(); index++) {
        auto ids = variables[index].ids();
        auto pixel = [this, &ids](){
            if (ids.size() == 1) {
                return m_pixels[*ids.begin()];
            } else if (ids.size() >= 2) {
                auto green = static_cast<uint8_t>((ids.size() * 255) / 64);
                return std::uint32_t(0xFF000000 | (green << 8));
            } else {
                return std::numeric_limits<std::uint32_t>::max();
            }
        }();
        pixels.emplace_back(std::move(pixel));
    }

    return pixels;
}

} // namespace wfc::model
