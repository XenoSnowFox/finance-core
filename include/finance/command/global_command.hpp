#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "finance/command/currency_commands.hpp"
#include "finance/command/documentation_commands.hpp"
#include "finance/command/parsers.hpp"

namespace finance::command {

/// @brief Root parser registering global options and top-level commands.
struct GlobalCommandParser : CommandParser {
  /// @brief Creates the finance-core command tree.
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
             std::make_shared<CurrencyCommandParser>(*this)},
            {CommandExample{"Display the available commands and options.",
                            "finance-core --help"}}} {}

  /// @brief Whether verbose logging is enabled by a global option.
  bool verbose_logging = false;
};

/// @brief Process-wide root command parser used by the application entry point.
inline GlobalCommandParser GLOBAL_COMMAND_PARSER;

} // namespace finance::command