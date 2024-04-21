#pragma once

#include "wfc/data/bit_set.h"

#include <cstddef>
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace wfc {

using Id = std::size_t;

using IdSet = data::BitSet;

template<typename T>
using IdMap = std::unordered_map<Id, T>;

} // namespace wfc
