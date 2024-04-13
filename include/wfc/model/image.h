#pragma once

#include "wfc/variable.h"
#include "wfc/data/tensor.h"
#include "wfc/interface/model.h"
#include "wfc/interface/image/loader.h"
#include "wfc/model/overlap/pattern.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

namespace wfc::model {

class Image: public interface::Model {
private:
    std::vector<Constraints> m_constraints;
    Model::Weights m_weights;
    IdMap<overlap::Pattern> m_patterns;
    IdMap<std::size_t> m_support_counts;

public:
    Image() = delete;
    Image(
        std::vector<Constraints> constraints,
        Model::Weights weights,
        IdMap<overlap::Pattern> patterns,
        IdMap<std::size_t> support_counts
    );

    Constraints& constraints(Id id) override;
    std::size_t constraint_offsets() const noexcept override;
    Weights weights() override;
    Variable make_variable() override;

    std::vector<image::Pixel> make_pixels(const data::Tensor<Variable>& variables);
};

} // namespace wfc::model
