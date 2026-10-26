#pragma once

#include "finance/command/command_parser.hpp"

namespace finance::command {

struct GlobalCommandParser : CommandParser {
  GlobalCommandParser()
      : CommandParser{
            "finance-core",
            "A self-hosted personal and solo business finance system."} {}

  /// @brief  special Global setting
  bool isSomething = false;
};

inline const GlobalCommandParser GLOBAL_COMMAND_PARSER{};

/*
    I would expect the following line of code to work.
    @code
    // n should equal "finance-core";
    std::string n = GLOBAL_COMMAND_PARSER.name;

    const CommandParser *p = &GLOBAL_COMMAND_PARSER;
    std::string x = p->name; // x should also equal "finance-core"


*/

} // namespace finance::command