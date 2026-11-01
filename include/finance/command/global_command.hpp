#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "finance/command/currency_commands.hpp"
#include "finance/command/database_commands.hpp"
#include "finance/command/documentation_commands.hpp"
#include "finance/command/parsers.hpp"

namespace finance::command {

/// @brief Root parser registering global options and top-level commands.
struct GlobalCommandParser : CommandParser {
  /// @brief Creates the finance-core command tree.
  GlobalCommandParser()
      : CommandParser{
            "finance_core",
            "A self-hosted personal and solo business finance system.",
            nullptr,
            {
                ArgumentParser{.keywords = {"-v", "--verbose"},
                               .argument_name = std::nullopt,
                               .summary = "Enable verbose logging.",
                               .callback =
                                   [this](std::queue<std::string> &args) {
                                     this->verbose_logging = true;
                                   }},
                ArgumentParser{.keywords = {"-d", "--database"},
                               .argument_name = "filename",
                               .summary = "Use this database file. Defaults to "
                                          "./default.db.",
                               .callback =
                                   [this](std::queue<std::string> &args) {
                                     this->database_filename = args.front();
                                     args.pop();
                                   }},
            },
            {
                std::make_shared<DocumentationCommandParser>(*this),
                std::make_shared<CurrencyCommandParser>(*this),
                std::make_shared<DatabaseCommandParser>(*this),
            },
            {
                CommandExample{"Display the available commands and options.",
                               "finance-core --help"},
            }} {}

  /// @brief Whether verbose logging is enabled by a global option.
  bool verbose_logging = false;

  /// @brief Database file selected by `-d` or `--database`.
  ///
  /// Defaults to `./default.db` when no database option is provided.
  std::string database_filename = "./default.db";
};

/// @brief Process-wide root command parser used by the application entry point.
inline GlobalCommandParser GLOBAL_COMMAND_PARSER;

} // namespace finance::command