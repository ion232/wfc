#pragma once

#include "wfc/id.h"

#include <unordered_map>

namespace wfc {

template<typename T>
using IdMap = std::unordered_map<Id, T>;

} // namespace wfc
