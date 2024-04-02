#pragma once

#include "wfc/domain.h"
#include "wfc/data/matrix.h"
#include "wfc/interface/model.h"

#include <cstdint>
#include <filesystem>
#include <vector>

namespace wfc::model {

class Image: public interface::Model {
private:
    IdMap<std::unordered_set<Id>> m_adjacencies;
    IdMap<std::size_t> m_weights;
    IdMap<uint32_t> m_pixels;

public:
    Image() = delete;
    Image(
        IdMap<std::unordered_set<Id>> adjacencies,
        IdMap<std::size_t> weights,
        IdMap<uint32_t> pixels
    );

    std::unordered_set<Id> lookup(Id id) override;
    IdMap<std::size_t> weights() override;
    std::size_t max_id() override;

    std::vector<uint32_t> make_pixels(const data::Matrix<Domain>& matrix);
};

std::optional<Image> load_image(std::filesystem::path path);

} // namespace wfc::model
