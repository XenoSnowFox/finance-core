#pragma once

#include "finance/command/parsers.hpp"

namespace finance::command {

struct CurrencyCommandParser : CommandParser {
  explicit CurrencyCommandParser(CommandParser &parent_command_parser)
      : CommandParser{
            "currency", "Command for managing currency information.",
            &parent_command_parser, {}, {}} {}
};

} // namespace finance::command