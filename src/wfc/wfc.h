#pragma once

#include "wfc/variable.h"
#include "wfc/data/tensor.h"
#include "wfc/io/random.h"
#include "wfc/interface/model.h"
#include "wfc/interface/heuristic/assignment.h"
#include "wfc/interface/heuristic/choice.h"

#include <memory>
#include <stack>
#include <vector>

namespace wfc {

class Wfc {
public:
    enum class Result {
        Ok,
        Resolved,
        InvalidChoice,
        InvalidAssignment,
        Contradiction
    };

    struct Config {
        std::shared_ptr<heuristic::interface::Assignment> assignment_heuristic;
        std::shared_ptr<heuristic::interface::Choice> choice_heuristic;
        std::shared_ptr<model::interface::Model> model;
        std::shared_ptr<io::Random> random;
    };

private:
    Config m_config;
    data::Tensor<Variable> m_variables;
    std::size_t m_variables_assigned;
    std::stack<std::size_t, std::vector<std::size_t>> m_propagation_stack;

public:
    Wfc(Config&& config, data::Tensor<Variable>&& variables);

    data::Tensor<Variable>& variables();

    Result step();

private:
    Result constrain();
    Result propagate();

    std::vector<Variable::Domain> constraining_domains(const Variable::Domain& target_domain);
    bool resolved();
};

} // namespace wfc
