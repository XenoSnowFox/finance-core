#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "finance/command/parsers.hpp"
#include "finance/core/domain/account.hpp"

namespace finance::command {

/**
 * @brief Parses and executes an account-add command for one account type.
 */
struct CreateAccountByTypeCommandParser : CommandParser {
  /**
   * @brief Creates a parser for adding one account classification.
   * @param arParentCommandParser Parser for the parent `accounts add` command.
   * @param aAccountTypeName Lowercase command name for the account type.
   * @param aType Classification assigned to the new account.
   */
  CreateAccountByTypeCommandParser(
      CommandParser &arParentCommandParser, std::string aAccountTypeName,
      core::domain::AccountType aType)
      : CommandParser{
            aAccountTypeName,
            "Adds a new account classified as " + aAccountTypeName + ".",
            &arParentCommandParser,
            {
                ArgumentParser{
                    .keywords = {"--currency"},
                    .argument_name = "CODE",
                    .summary = "Currency code used by the new account.",
                    .callback =
                        [this](std::queue<std::string> &arArgs) {
                          if (arArgs.empty()) {
                            std::cerr << "Error: --currency requires a "
                                         "value.\n";
                            this->mOptionError = true;
                            return;
                          }

                          this->mCurrencyCode = arArgs.front();
                          arArgs.pop();
                        },
                },
                ArgumentParser{
                    .keywords = {"--code"},
                    .argument_name = "IDENTIFIER",
                    .summary = "Identifier for the new account.",
                    .callback =
                        [this](std::queue<std::string> &arArgs) {
                          if (arArgs.empty()) {
                            std::cerr << "Error: --code requires a value.\n";
                            this->mOptionError = true;
                            return;
                          }

                          this->mIdentifier = arArgs.front();
                          arArgs.pop();
                        },
                },
            },
            {},
            {
                CommandExample{
                    "Add a new " + aAccountTypeName + " account.",
                    "finance_core accounts add " + aAccountTypeName +
                        " --currency USD --code ACCOUNT_CODE \"Account Name\""},
            },
            "NAME",
        },
        mType(aType) {}

  /**
   * @brief Clears option state before parsing a new command invocation.
   * @param arArgs Command-line tokens to consume.
   * @return The account-add command result.
   */
  int parse(std::queue<std::string> &arArgs) const override {
    mCurrencyCode.reset();
    mIdentifier.reset();
    mOptionError = false;
    return CommandParser::parse(arArgs);
  }

protected:
  /**
   * @brief Creates and persists the account described by the parsed inputs.
   * @param aPositionalArgument Human-readable name for the new account.
   * @return 0 if the account was added; 1 if required input is missing, its
   * currency is not in the book, or its identifier already exists.
   */
  int executeCommand(
      const std::optional<std::string> &aPositionalArgument) const override;

private:
  /**
   * @brief Account classification selected by this parser's command name.
   */
  const core::domain::AccountType mType;

  /**
   * @brief Currency code supplied by the `--currency` option.
   */
  mutable std::optional<std::string> mCurrencyCode;

  /**
   * @brief Account identifier supplied by the `--code` option.
   */
  mutable std::optional<std::string> mIdentifier;

  /**
   * @brief Whether an option callback encountered a missing value.
   */
  mutable bool mOptionError = false;
};

/**
 * @brief Parses and executes the `accounts add` command.
 */
struct AddAccountCommandParser : CommandParser {
  /**
   * @brief Creates a parser for `accounts add` and its account types.
   * @param arParentCommandParser Parser for the parent `accounts` command.
   */
  explicit AddAccountCommandParser(CommandParser &arParentCommandParser)
      : CommandParser{
            "add",
            "Adds a new account to the book.",
            &arParentCommandParser,
            {},
            {
                std::make_shared<CreateAccountByTypeCommandParser>(
                    *this, "asset", core::domain::AccountType::ASSET),
                std::make_shared<CreateAccountByTypeCommandParser>(
                    *this, "liability", core::domain::AccountType::LIABILITY),
                std::make_shared<CreateAccountByTypeCommandParser>(
                    *this, "equity", core::domain::AccountType::EQUITY),
                std::make_shared<CreateAccountByTypeCommandParser>(
                    *this, "income", core::domain::AccountType::INCOME),
                std::make_shared<CreateAccountByTypeCommandParser>(
                    *this, "expense", core::domain::AccountType::EXPENSE),
            },
        } {}
};

/**
 * @brief Parses and executes the `accounts list` command.
 */
struct ListAccountCommandParser : CommandParser {
  /**
   * @brief Creates a parser for the `accounts list` command.
   * @param arParentCommandParser Parser for the parent `accounts` command.
   */
  explicit ListAccountCommandParser(CommandParser &arParentCommandParser)
      : CommandParser{
            "list",
            "Lists all accounts, optionally filtered by account type.",
            &arParentCommandParser,
            {},
            {},
            {
                CommandExample{"List all accounts defined.",
                               "finance_core accounts list"},
                CommandExample{"List accounts of a specific type.",
                               "finance_core accounts list asset"},
            },
            "TYPE",
        } {}

protected:
  /**
   * @brief Prints all accounts or accounts matching the optional type filter.
   * @param aPositionalArgument Lowercase account type to list, if supplied.
   * @return 0 after the list completes; 1 if the supplied type is unsupported.
   */
  int executeCommand(
      const std::optional<std::string> &aPositionalArgument) const override;
};

/**
 * @brief Parses the top-level `accounts` command and its subcommands.
 */
struct AccountCommandParser : CommandParser {
  /**
   * @brief Creates a parser for the `accounts` command.
   * @param arParentCommandParser Parser for the parent command.
   */
  explicit AccountCommandParser(CommandParser &arParentCommandParser)
      : CommandParser{
            "accounts",
            "Command for managing account information.",
            &arParentCommandParser,
            {},
            {
                std::make_shared<AddAccountCommandParser>(*this),
                std::make_shared<ListAccountCommandParser>(*this),
            },
        } {}
};

} // namespace finance::command