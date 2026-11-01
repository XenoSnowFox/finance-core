#pragma once

#include <optional>
#include <string>

#include "finance/command/parsers.hpp"

namespace finance::command {

/// @brief Creates a new empty SQLite database file.
struct CreateDatabaseCommandParser : CommandParser {
  /// @brief Creates a database parser attached to its parent command.
  /// @param parent_command_parser Parser that owns the `create` subcommand.
  explicit CreateDatabaseCommandParser(CommandParser &parent_command_parser)
      : CommandParser{"create",
                      "Create a new database file.",
                      &parent_command_parser,
                      {},
                      {},
                      {
                          CommandExample{
                              "Create the specified database file.",
                              "finance_core database create ./finance.db"},
                          CommandExample{
                              "Create the database file selected by a global "
                              "option.",
                              "finance_core --database ./finance.db database "
                              "create"},
                      },
                      PositionalArgumentParser{
                          "filename",
                          "Database file to create. Overrides `-d` or "
                          "`--database` when provided.",
                          [this](const std::string &filename) {
                            this->positional_filename = filename;
                          }}} {}

  /// @brief Creates the selected database file without initializing a schema.
  /// @return `0` on success; `1` if the database file cannot be created.
  int executeCommand() const override;

  /// @brief Optional database filename supplied after the `create` command.
  std::optional<std::string> positional_filename;
};

/// @brief Registers database-management subcommands.
struct DatabaseCommandParser : CommandParser {
  /// @brief Creates the database management parser.
  /// @param parent_command_parser Parser that owns the `database` command.
  explicit DatabaseCommandParser(CommandParser &parent_command_parser)
      : CommandParser{"database",
                      "Database management",
                      &parent_command_parser,
                      {},
                      {
                          std::make_shared<CreateDatabaseCommandParser>(
                              *this),
                      }} {}
};

} // namespace finance::command