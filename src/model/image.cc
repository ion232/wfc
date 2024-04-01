#include "wfc/model/image.h"

namespace wfc::model {

Image::Image(std::filesystem::path path)
    : m_path(std::move(path))
{}

std::unordered_set<Id> Image::lookup(Id id) {
    return {};
}

std::vector<uint32_t> Image::make_pixels(const data::Matrix<Domain>& variables) {
    return {};
}

} // namespace wfc::model::image
