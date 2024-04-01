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
    std::filesystem::path m_path;

public:
    Image() = delete;
    Image(std::filesystem::path path);

    std::unordered_set<Id> lookup(Id id) override;

    std::vector<uint32_t> make_pixels(const data::Matrix<Domain>& matrix);
};

} // namespace wfc::model
