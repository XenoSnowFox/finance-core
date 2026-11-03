#pragma once

#include <optional>
#include <string>

#include "finance/command/parsers.hpp"

namespace finance::command {

/**
 * @brief Parses the `book create` command.
 */
struct BookCreateCommandParser : CommandParser {
  /**
   * @brief Creates a parser for the `book create` command.
   * @param parent_command_parser Parser for the parent `book` command.
   */
  explicit BookCreateCommandParser(CommandParser &parent_command_parser)
      : CommandParser{
            "create",
            "Create a new book file.",
            &parent_command_parser,
            {},
            {},
            {CommandExample{"Create a book file at the specified path.",
                            "finance_core book create ./my-finances.book"},
             CommandExample{
                 "Create the selected book file.",
                 "finance_core --book ./my-finance.book book create"}},
            "filename"} {}

protected:
  /**
   * @brief Creates a Book file at the supplied or globally selected path.
   * @param aPositionalArgument Optional filename supplied after `create`.
   * @return `0` after the file is written.
   */
  int executeCommand(
      const std::optional<std::string> &aPositionalArgument) const override;
};

/**
 * @brief Parses commands for managing a Book.
 */
struct BookCommandParser : CommandParser {
  /**
   * @brief Creates a parser for Book management commands.
   * @param parent_command_parser Parser that owns the `book` command.
   */
  explicit BookCommandParser(CommandParser &parent_command_parser)
      : CommandParser{"book",
                      "Command for managing a book of finance.",
                      &parent_command_parser,
                      {},
                      {std::make_shared<BookCreateCommandParser>(*this)}} {}
};

} // namespace finance::command
