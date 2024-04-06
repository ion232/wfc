#pragma once

#include "wfc/id.h"
#include "wfc/io/random.h"

#include <functional>
#include <unordered_set>
#include <span>
#include <vector>

namespace wfc {

class Domain {
private:
    IdSet m_set;

public:
    Domain();
    Domain(Id max_id);

    void assign(Id id);
    bool constrain_to(const IdSet& allowed);

    IdSet ids() const noexcept;
    std::size_t size() const noexcept;
};

} // namespace wfc
