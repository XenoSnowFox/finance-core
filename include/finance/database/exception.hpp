#pragma once

#include <stdexcept>
#include <string>

/// @file
/// @brief Base exception type for database errors.

namespace finance::database {
/// @brief Base class for all errors raised by database backends.
///
/// Carries the backend-specific numeric error code alongside the message.
class Exception : public std::runtime_error {
public:
  /// @brief Constructs an exception from a C string message.
  /// @param error_message Description of the error; null is treated as empty.
  /// @param error_code Backend-specific error code.
  Exception(const char *error_message, const int error_code)
      : std::runtime_error(error_message ? error_message : ""),
        m_error_code(error_code) {}

  /// @brief Constructs an exception from a std::string message.
  /// @param error_message Description of the error.
  /// @param error_code Backend-specific error code.
  Exception(const std::string &error_message, const int error_code)
      : Exception(error_message.c_str(), error_code) {}

  /// @brief Returns the backend-specific numeric error code.
  /// @return Stored backend-specific error code.
  const int getErrorCode() const noexcept { return this->m_error_code; }

protected:
  /// @brief Backend-specific numeric error code stored by this exception.
  const int m_error_code;
};
} // namespace finance::database