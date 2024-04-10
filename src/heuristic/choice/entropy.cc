#include "wfc/heuristic/choice/entropy.h"

#include <limits>

namespace wfc::heuristic::choice {
namespace {
    float calculate_entropy(std::vector<std::size_t>&& weights) {
       auto entropy = float(0);
       auto total_weight = float(0);

       for (const auto& w : weights) {
           total_weight += w;
       }

       for (const auto& w : weights) {
           if (w == 0) {
               continue;
           }
           auto p = w / total_weight;
           entropy -= p * std::log2f(p);
       }

        return entropy;
    }
}

Entropy::Entropy(
    std::shared_ptr<Weights> weights,
    std::shared_ptr<io::Random> random
)
    : m_weights(std::move(weights))
    , m_random(std::move(random))
    , m_undecided()
{}

void Entropy::inform(std::size_t index, const wfc::Variable& variable) {
    if (auto it = m_undecided.find(index); it != m_undecided.end()) {
        m_undecided.erase(it);
    }
    
    if (variable.state() == wfc::Variable::State::Undecided) {
        auto weights = m_weights->of(variable.ids());
        auto entropy = calculate_entropy(std::move(weights));
        m_undecided[index] = entropy;
    }
}

std::optional<std::size_t> Entropy::choose() {
    auto variable_indices = std::vector<std::size_t>();
    auto min_entropy = std::numeric_limits<float>::max();

    for (auto it = m_undecided.begin(); it != m_undecided.end(); it++) {
        const auto entropy = it->second;
        if (entropy < min_entropy) {
            min_entropy = entropy;
            variable_indices.clear();
        }
        if (entropy == min_entropy) {
            variable_indices.emplace_back(it->first);
        }
    }
    
    if (variable_indices.size() == 0) {
        return std::nullopt;
    }

    if (variable_indices.size() == 1) {
        return *variable_indices.begin();
    }

    const auto random_index = m_random->make_size_t(0, variable_indices.size() - 1);
    const auto variable_index = variable_indices[random_index];
    m_undecided.erase(variable_index);

    return variable_index;
}

} // namespace wfc::heuristic::choice
