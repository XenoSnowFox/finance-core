#pragma once

#include <sqlite3.h>

#include "finance/database/exception.hpp"

/// @file
/// @brief SQLite3-specific exception type.

namespace finance::database::sqlite3 {

/// @brief Exception raised for SQLite3 failures.
///
/// The message is taken from the connection's last error (sqlite3_errmsg).
class Exception : public database::Exception {
public:
  /// @brief Constructs from a connection's most recent error.
  /// @param sqlite3_instance Open connection; message and code are read from
  /// it.
  explicit Exception(sqlite3 *sqlite3_instance)
      : database::Exception(sqlite3_errmsg(sqlite3_instance),
                            sqlite3_errcode(sqlite3_instance)) {}

  /// @brief Constructs with an explicit error code.
  /// @param sqlite3_instance Open connection; the message is read from it.
  /// @param error_code SQLite result code to store (e.g. from a failed call).
  explicit Exception(sqlite3 *sqlite3_instance, int error_code)
      : database::Exception(sqlite3_errmsg(sqlite3_instance), error_code) {}

  /// @brief Returns the generic SQLite description of the stored error code.
  /// @return Static string from sqlite3_errstr; owned by SQLite.
  const char *getErrorString() const noexcept {
    return sqlite3_errstr(this->m_error_code);
  }
};

} // namespace finance::database::sqlite3