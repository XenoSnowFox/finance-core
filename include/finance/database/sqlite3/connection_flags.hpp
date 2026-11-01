#pragma once

#include <sqlite3.h>

namespace finance::database::sqlite3 {
/// @brief SQLite flags used when opening a database connection.
///
/// Each value maps to a SQLite `SQLITE_OPEN_*` flag. Multiple values can be
/// supplied together to configure a connection.
enum class ConnectionFlag {
  /// @brief Open an existing database in read-only mode.
  READ_ONLY = SQLITE_OPEN_READONLY,

  /// @brief Open an existing database for reading and writing, falling back to
  /// read-only access if opening it for writing fails.
  READ_WRITE = SQLITE_OPEN_READWRITE,

  /// @brief Create the database if it does not exist when used with
  /// `READ_WRITE`.
  CREATE = SQLITE_OPEN_CREATE,

  /// @brief Causes the filename parameter to be interpreted as a URI according
  /// to RFC 3986 guidelines.
  URI = SQLITE_OPEN_URI,

  /// @brief Open the database as an in-memory database rather than an on-disk
  /// file.
  MEMORY = SQLITE_OPEN_MEMORY,

  /// @brief  Configures the database connection to return extended error codes
  /// instead of generic primary result codes.
  EXTENDED_RESULT_CODES = SQLITE_OPEN_EXRESCODE,

};
}