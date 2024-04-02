#pragma once

#include <cstddef>
#include <unordered_map>

namespace wfc {

using Id = std::size_t;

template<typename T>
using IdMap = std::unordered_map<Id, T>;

} // namespace wfc::model
