#pragma once

#include "wfc/id.h"
#include "wfc/id_set.h"
#include "wfc/io/random.h"

#include <functional>
#include <unordered_set>
#include <span>
#include <vector>

namespace wfc {

class Variable {
public:
    using Domain = IdSet;

    enum class State {
        Invalid,
        Undecided,
        Decided
    };

private:
    Domain m_domain;

public:
    Variable();
    Variable(Domain&& domain);

    void assign(Id id);
    void remove(Id id);
    bool constrain_to(const Domain& allowed);

    Domain domain() const noexcept;
    State state() const noexcept;
    std::size_t size() const noexcept;

    bool operator==(const Variable& other) const;
};

} // namespace wfc
