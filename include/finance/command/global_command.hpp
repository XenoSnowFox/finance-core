#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "finance/command/account_commands.hpp"
#include "finance/command/book_commands.hpp"
#include "finance/command/currency_commands.hpp"
#include "finance/command/documentation_commands.hpp"
#include "finance/command/parsers.hpp"

namespace finance::command {

/**
 * @brief Root parser registering global options and top-level commands.
 */
struct GlobalCommandParser : CommandParser {
  /**
   * @brief Creates the finance-core command tree.
   */
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
                                   [this](std::queue<std::string> &) {
                                     this->verbose_logging = true;
                                   }},
                ArgumentParser{
                    .keywords = {"-b", "--book"},
                    .argument_name = "filename",
                    .summary =
                        "Filename of the book of finance to "
                        "execute commands on. Defaults to './default.book'.",
                    .callback =
                        [this](std::queue<std::string> &args) {
                          const std::string filename = args.front();
                          args.pop();
                          this->book_filename = filename;
                        }},
            },
            {
                std::make_shared<DocumentationCommandParser>(*this),
                std::make_shared<CurrencyCommandParser>(*this),
                std::make_shared<AccountCommandParser>(*this),
                std::make_shared<BookCommandParser>(*this),
            },
            {
                CommandExample{"Display the available commands and options.",
                               "finance-core --help"},
                CommandExample{
                    "Select a specific book file.",
                    "finance-core --book my-finances.book [COMMAND]"},
            },
        } {}

  /**
   * @brief Whether verbose logging is enabled by a global option.
   */
  bool verbose_logging = false;

  std::string book_filename = "./default.book";
};

/**
 * @brief Process-wide root command parser used by the application entry point.
 */
inline GlobalCommandParser GLOBAL_COMMAND_PARSER;

} // namespace finance::command