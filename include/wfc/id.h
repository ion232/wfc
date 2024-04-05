#pragma once

#include <cstddef>
#include <unordered_set>
#include <unordered_map>

namespace wfc {

using Id = std::size_t;

template<typename T>
using IdMap = std::unordered_map<Id, T>;

using IdSet = std::unordered_set<Id>;

} // namespace wfc::model
