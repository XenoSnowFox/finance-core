#pragma once

#include <sqlite3.h>

#include "finance/database/exception.hpp"
#include "finance/database/sqlite3/error_code.hpp"

/// @file
/// @brief SQLite3-specific exception type.

namespace finance::database::sqlite3 {

/// @brief Exception raised for SQLite failures.
///
/// The message is taken from the connection's current error
/// (sqlite3_errmsg). The exception also retains SQLite's extended error code.
class Exception : public database::Exception {
public:
  /// @brief Constructs from a connection's current error.
  /// @param sqlite3_instance SQLite connection from which the message,
  /// primary code, and extended code are read.
  explicit Exception(::sqlite3 *sqlite3_instance)
      : database::Exception(sqlite3_errmsg(sqlite3_instance),
                            sqlite3_errcode(sqlite3_instance)),
        m_extended_error_code(sqlite3_extended_errcode(sqlite3_instance)) {}

  /// @brief Constructs with an explicit result code and the connection's
  /// current message and extended code.
  /// @param sqlite3_instance SQLite connection from which the message and
  /// extended code are read.
  /// @param error_code SQLite result code stored as the exception's primary
  /// code.
  explicit Exception(::sqlite3 *sqlite3_instance, int error_code)
      : database::Exception(sqlite3_errmsg(sqlite3_instance), error_code),
        m_extended_error_code(sqlite3_extended_errcode(sqlite3_instance)) {}

  /// @brief Returns the generic SQLite description of the stored error code.
  /// @return Static string from sqlite3_errstr; owned by SQLite.
  const char *getErrorString() const noexcept {
    return sqlite3_errstr(this->m_error_code);
  }

  /// @brief Returns SQLite's extended result code captured at construction.
  /// @return Extended SQLite result code.
  const int getExtendedErrorCode() const noexcept {
    return this->m_extended_error_code;
  }

  /// @brief Returns the generic SQLite description of the extended error code.
  /// @return Description returned by sqlite3_errstr.
  const std::string getExtendedErrorString() const noexcept {
    return sqlite3_errstr(this->m_extended_error_code);
  }

  /// @brief Tests whether either stored result code matches a recognized code.
  /// @param error_code SQLite result code to compare.
  /// @return `true` if the primary or extended result code matches.
  const bool is(const ErrorCode error_code) const noexcept {
    return static_cast<int>(error_code) == this->m_error_code ||
           static_cast<int>(error_code) == this->m_extended_error_code;
  }

protected:
  /// Extended SQLite result code captured from the connection at construction.
  const int m_extended_error_code;
};

} // namespace finance::database::sqlite3