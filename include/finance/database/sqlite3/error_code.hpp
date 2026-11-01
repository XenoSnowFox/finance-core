#pragma once

#include <sqlite3.h>

namespace finance::database::sqlite3 {

/// @brief SQLite result codes recognized by this library.
enum class ErrorCode {
  /// @brief Generic error used when no more specific result code is available.
  ERROR = SQLITE_ERROR,

  /// @brief SQLite was unable to open a database or temporary file.
  CANT_OPEN = SQLITE_CANTOPEN,
};

}