#pragma once

#include "wfc/id.h"
#include "wfc/io/random.h"

#include <functional>
#include <unordered_set>
#include <span>
#include <vector>

namespace wfc {

class Domain {
public:
    using Set = std::unordered_set<Id>;
    using ScoreFunction = std::function<float(const Set&)>;

private:
    Set m_set;

public:
    Domain();
    Domain(Id max_id);

    void choose(Id id);
    bool constrain_to(const Set& allowed);

    Set ids();
    std::size_t size();
};

} // namespace wfc
