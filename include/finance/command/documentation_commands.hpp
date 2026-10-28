#pragma once

#include "finance/command/parsers.hpp"

namespace finance::command {

struct DocumentationCommandParser : CommandParser {
  explicit DocumentationCommandParser(CommandParser &parent_command_parser)
      : CommandParser{"doc", "Generate documentation for the tool.",
                      &parent_command_parser, {}, {}} {}
};

} // namespace finance::command