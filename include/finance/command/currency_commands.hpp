#pragma once

#include <charconv>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <system_error>

#include "finance/command/parsers.hpp"
#include "finance/core/domain/currency.hpp"

namespace finance::command {

/**
 * @brief Parses and executes the `currency add` command.
 *
 * Decimal places must be an integer from 0 through 10.
 */
struct AddCurrencyCommandParser : CommandParser {
  /**
   * @brief Creates a parser for the `currency add` command.
   * @param arParentCommandParser Parser for the parent `currency` command.
   */
  explicit AddCurrencyCommandParser(CommandParser &arParentCommandParser)
      : CommandParser{
            "add",
            "Adds a new currency to the book.",
            &arParentCommandParser,
            {
                ArgumentParser{
                    .keywords = {"--symbol"},
                    .argument_name = "symbol",
                    .summary = "Symbol used to represent the new currency "
                               "being added to the book. Defaults to '$'",
                    .callback =
                        [this](std::queue<std::string> &arArgs) {
                          const std::string symbol = arArgs.front();
                          arArgs.pop();
                          this->mSymbol = symbol;
                        },
                },
                ArgumentParser{
                    .keywords = {"--decimalPlaces"},
                    .argument_name = "decimalPlaces",
                    .summary = "Number of decimal places to represent the "
                               "fractional part of the currency. Defaults to 2",
                    .callback =
                        [this](std::queue<std::string> &arArgs) {
                          if (arArgs.empty()) {
                            std::cerr << "Error: --decimalPlaces requires a "
                                         "value.\n";
                            this->mOptionError = true;
                            return;
                          }

                          const std::string decimalPlacesText = arArgs.front();
                          arArgs.pop();
                          int decimalPlaces = 0;
                          const char *const inputEnd =
                              decimalPlacesText.data() +
                              decimalPlacesText.size();
                          const auto conversion =
                              std::from_chars(decimalPlacesText.data(),
                                              inputEnd, decimalPlaces);

                          if (conversion.ec != std::errc{} ||
                              conversion.ptr != inputEnd || decimalPlaces < 0 ||
                              decimalPlaces > 10) {
                            std::cerr << "Error: --decimalPlaces must be an "
                                         "integer from 0 to 10.\n";
                            this->mOptionError = true;
                            return;
                          }

                          this->mDecimalPlaces =
                              static_cast<std::uint8_t>(decimalPlaces);
                        },
                },

            },
            {},
            {
                CommandExample{"Add a new currency.",
                               "finance_core currency add USD --symbol $ "
                               "--decimalPlaces 2"},
            },
            "code",
        } {}

protected:
  /**
   * @brief Validates and persists one currency.
   * @param aPositionalArgument Three-character currency code; missing or
   * differently sized codes are rejected.
   * @return 0 if the currency was added; 1 if input is invalid or the code
   * already exists.
   */
  int executeCommand(
      const std::optional<std::string> &aPositionalArgument) const override;

private:
  /**
   * @brief Symbol supplied for the new currency; absence uses "$".
   */
  mutable std::optional<std::string> mSymbol;

  /**
   * @brief Supplied fractional digit count, from 0 through 10; absence uses 2.
   */
  mutable std::optional<std::uint8_t> mDecimalPlaces;

  /**
   * @brief Whether an option callback encountered invalid input.
   */
  mutable bool mOptionError = false;
};

/**
 * @brief Parses and executes the `currency list` command.
 *
 * Lists currencies in case-sensitive alphabetical order by code.
 */
struct ListCurrencyCommandParser : CommandParser {
  /**
   * @brief Creates a parser for the `currency list` command.
   * @param arParentCommandParser Parser for the parent `currency` command.
   */
  explicit ListCurrencyCommandParser(CommandParser &arParentCommandParser)
      : CommandParser{
            "list",
            "List all the currencies available within the book.",
            &arParentCommandParser,
            {},
            {},
            {
                CommandExample{"List all the currencies defined.",
                               "finance_core currency list"},
            },
        } {}

protected:
  /**
   * @brief Prints all currencies defined in the configured Book.
   * @return 0 after the list command completes.
   */
  int executeCommand() const override;
};

/**
 * @brief Parses the top-level `currency` command and its subcommands.
 */
struct CurrencyCommandParser : CommandParser {
  /**
   * @brief Creates a parser for the `currency` command.
   * @param arParentCommandParser Parser for the parent command.
   */
  explicit CurrencyCommandParser(CommandParser &arParentCommandParser)
      : CommandParser{
            "currency",
            "Command for managing currency information.",
            &arParentCommandParser,
            {},
            {
                std::make_shared<AddCurrencyCommandParser>(*this),
                std::make_shared<ListCurrencyCommandParser>(*this),
            },
        } {}
};

} // namespace finance::command