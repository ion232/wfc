#pragma once

#include "wfc/domain.h"
#include "wfc/data/matrix.h"
#include "wfc/io/random.h"
#include "wfc/interface/model.h"
#include "wfc/interface/heuristic/assignment.h"
#include "wfc/interface/heuristic/variable.h"

#include <memory>
#include <stack>
#include <vector>

namespace wfc {

class Wfc {
public:
    struct Config {
        std::shared_ptr<heuristic::interface::Assignment> assignment_heuristic;
        std::shared_ptr<heuristic::interface::Variable> variable_heuristic;
        std::shared_ptr<model::interface::Model> model;
        std::shared_ptr<io::Random> random;
    };

private:
    Config m_config;
    data::Matrix<Domain> m_variables;
    std::size_t m_variables_assigned;
    std::stack<std::size_t, std::vector<std::size_t>> m_propagation_stack;

public:
    Wfc(Config&& config, data::Matrix<Domain>&& variables);

    const data::Matrix<Domain>& variables();

    bool step();

private:
    void constrain();
    void propagate();

    std::vector<std::unordered_set<Id>> valid_adjacencies(const Domain::Set& ids);
    bool resolved();
};

} // namespace wfc
