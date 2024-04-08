#pragma once

#include "wfc/domain.h"
#include "wfc/data/matrix.h"
#include "wfc/interface/model.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

namespace wfc::model {

class Image: public interface::Model {
private:
    IdMap<Model::Adjacencies> m_adjacencies;
    Model::Weights m_weights;
    IdMap<std::uint32_t> m_pixels;
    IdMap<std::size_t> m_support_counts;

public:
    Image() = delete;
    Image(
        IdMap<Model::Adjacencies> adjacencies,
        Model::Weights weights,
        IdMap<std::uint32_t> pixels,
        IdMap<std::size_t> support_counts
    );

    Model::Adjacencies lookup(Id id) override;
    std::size_t adjacent_count() override;
    Model::Weights weights() override;
    Domain initial_domain() override;

    std::vector<std::uint32_t> make_pixels(const data::Matrix<Domain>& matrix);
};

std::optional<Image> load_image(std::filesystem::path path);

} // namespace wfc::model
