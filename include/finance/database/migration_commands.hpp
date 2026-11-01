#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace finance::database {

/// @brief SQL column types used by schema migration commands.
///
/// The enum identifies a database type family; it does not specify
/// precision, length, or database-specific type behavior.
enum class ColumnType {
  /// @brief Integer type with database-defined range.
  INTEGER,
  /// @brief Small integer type with database-defined range.
  TINYINT,
  /// @brief Large integer type with database-defined range.
  BIGINT,
  /// @brief Variable-length text type.
  TEXT,
  /// @brief Fixed-length character type.
  CHAR,
  /// @brief Variable-length character type.
  VARCHAR,
  /// @brief Binary large-object type.
  BLOB,
  /// @brief Real-number type.
  REAL,
  /// @brief Floating-point type.
  FLOAT,
  /// @brief Double-precision floating-point type.
  DOUBLE,
  /// @brief Boolean type.
  BOOLEAN,
  /// @brief Numeric type with database-defined precision.
  NUMERIC,
  /// @brief Decimal type with database-defined precision.
  DECIMAL,
  /// @brief Date type.
  DATE,
  /// @brief Date-and-time type.
  DATETIME,
  /// @brief Timestamp type.
  TIMESTAMP,
};

/// @brief Identifies a referenced table column for a foreign key.
///
/// The reference stores only the target names; it does not specify
/// referential actions such as cascade behavior.
struct ForeignKeyReference {
  /// @brief Name of the table containing the referenced column.
  std::string tableName;

  /// @brief Name of the referenced column.
  std::string column;
};

/// @brief Describes a column in a table schema.
///
/// This record captures basic column attributes and foreign-key targets; it
/// does not represent every constraint or database-specific type option.
struct Column {
  /// @brief Name of the column.
  std::string name;

  /// @brief Optional descriptive comment for the column.
  std::string comment = "";

  /// @brief SQL type family of the column.
  ColumnType type;

  /// @brief Whether the column accepts null values.
  bool nullable{true};

  /// @brief Whether the column is a primary key.
  bool primaryKey{false};

  /// @brief Whether the column uses automatic value generation.
  bool autoIncrement{false};

  /// @brief Foreign-key targets associated with the column.
  std::vector<ForeignKeyReference> foreignKeys;

  /// @brief Optional SQL default expression or literal.
  std::optional<std::string> default_value = std::nullopt;
};

/// @brief Describes a command that creates a table and its columns.
struct CreateTableCommand {
  /// @brief Name of the table to create.
  std::string name;

  /// @brief Columns to define in the new table.
  std::vector<Column> columns;
};

/// @brief Describes a command that adds one column to a table.
struct AddColumnCommand {
  /// @brief Name of the table to alter.
  std::string tableName;

  /// @brief Column to add to the table.
  Column column;
};

/// @brief Describes a command that drops a table.
struct DropTableCommand {
  /// @brief Name of the table to drop.
  std::string name;
};

/// @brief Describes a command that creates an index over table columns.
///
/// The record supports a column-based index and a uniqueness flag; it does not
/// describe index methods or other database-specific index options.
struct CreateIndexCommand {
  /// @brief Name of the index to create.
  std::string indexName;

  /// @brief Name of the table to index.
  std::string tableName;

  /// @brief Names of the columns included in the index.
  std::vector<std::string> columns;

  /// @brief Whether the index enforces unique values.
  bool unique{false};
};

/// @brief One supported schema operation in a migration.
///
/// Only table creation, column addition, table removal, and index creation are
/// represented by this variant.
using MigrationCommand = std::variant<CreateTableCommand, AddColumnCommand,
                                      DropTableCommand, CreateIndexCommand>;

/// @brief Groups schema commands and metadata for one domain migration.
struct DomainMigration {
  /// @brief Domain or schema area associated with the migration.
  std::string domain;

  /// @brief Notes describing the migration.
  std::string migration_notes{""};

  /// @brief Version number identifying this migration.
  std::int64_t version{0};

  /// @brief Schema commands included in the migration.
  std::vector<MigrationCommand> commands;
};

} // namespace finance::database