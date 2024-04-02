#include "wfc/model/image.h"

#include <array>

#include <png.h>

namespace wfc::model {

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
    
}

} // namespace wfc::model::image
