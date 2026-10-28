#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "finance/command/currency_commands.hpp"
#include "finance/command/documentation_commands.hpp"
#include "finance/command/parsers.hpp"

namespace finance::command {

struct GlobalCommandParser : CommandParser {
  GlobalCommandParser()
      : CommandParser{
            "finance-core",
            "A self-hosted personal and solo business finance system.",
            nullptr,
            {ArgumentParser{.keywords = {"-v", "--verbose"},
                            .argument_name = std::nullopt,
                            .summary = "Enable verbose logging.",
                            .callback =
                                [this](std::queue<std::string> &args) {
                                  this->verbose_logging = true;
                                }}},
            {std::make_shared<DocumentationCommandParser>(*this),
             std::make_shared<CurrencyCommandParser>(*this)}} {}

  /// @brief  special Global setting
  bool verbose_logging = false;
};

inline GlobalCommandParser GLOBAL_COMMAND_PARSER;

} // namespace finance::command