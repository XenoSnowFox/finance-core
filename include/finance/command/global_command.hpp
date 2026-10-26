#pragma once

#include <string>

#include "finance/command/command_parser.hpp"

namespace finance::command {

inline const CommandParser GLOBAL_COMMAND_PARSER{
    .name = "finance-core",
    .summary = "A self-hosted personal and solo business finance system."};

} // namespace finance::command