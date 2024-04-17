#pragma once

#include "wfc/variable.h"
#include "wfc/data/tensor.h"
#include "wfc/interface/model.h"
#include "wfc/interface/image/loader.h"
#include "wfc/model/overlap/pattern.h"

#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <vector>

namespace wfc::model {

class Image: public interface::Model {
public:
    using Patterns = IdMap<overlap::Pattern>;

private:
    std::vector<Constraints> m_constraints;
    Weights m_weights;
    Patterns m_patterns;

public:
    Image() = delete;
    Image(
        std::vector<Constraints> constraints,
        Model::Weights weights,
        IdMap<overlap::Pattern> patterns
    );

    data::Tensor<Variable> make_variables(const std::vector<data::Dimension>& dimensions) override;
    Constraints& constraints(Id id) override;
    std::size_t constraint_offsets() const noexcept override;
    Weights weights() override;

    const Patterns& patterns() const noexcept;

    std::vector<image::Pixel> make_pixels(const data::Tensor<Variable>& variables);
};

} // namespace wfc::model
