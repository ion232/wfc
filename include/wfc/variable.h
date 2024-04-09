#pragma once

#include "wfc/id.h"
#include "wfc/io/random.h"

#include <functional>
#include <unordered_set>
#include <span>
#include <vector>

namespace wfc {

class Variable {
private:
    IdMap<std::size_t> m_map;

public:
    Variable();
    Variable(IdMap<std::size_t>&& map);

    void assign(Id id);
    bool constrain_to(const IdSet& allowed);

    IdSet ids() const noexcept;
    std::size_t size() const noexcept;

    bool operator==(const Variable& other) const;
};

} // namespace wfc
