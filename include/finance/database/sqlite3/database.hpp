#pragma once

#include <cassert>
#include <memory>
#include <sqlite3.h>
#include <string>
#include <vector>

#include "finance/database/database.hpp"
#include "finance/database/sqlite3/connection_flags.hpp"
#include "finance/database/sqlite3/exception.hpp"

namespace finance::database::sqlite3 {
/// @brief SQLite's success result code.
extern const int OK = SQLITE_OK;

/// @brief RAII wrapper around a SQLite database connection.
///
/// The connection is closed when the `Database` object is destroyed.
/// Connections are opened read-only by default.
class Database : public database::Database {
public:
  /**
   * @brief Open a SQLite database connection.
   *
   * @param filename Path of the database to open, or a URI filename when
   * `ConnectionFlag::URI` is included in `flags`.
   * @param flags SQLite connection flags. Defaults to
   * `ConnectionFlag::READ_ONLY`.
   * @param busy_timeout_in_milliseconds Positive busy timeout to set after
   * opening, in milliseconds. Non-positive values leave SQLite's default
   * timeout unchanged.
   * @throws Exception if SQLite cannot open the database or set the requested
   * busy timeout.
   */
  Database(
      const std::string &filename,
      const std::vector<ConnectionFlag> flags = {ConnectionFlag::READ_ONLY},
      const int busy_timeout_in_milliseconds = 0)
      : m_filename(filename) {

    int connection_flags = 0;
    for (auto &flag : flags) {
      connection_flags |= static_cast<int>(flag);
    }

    ::sqlite3 *handle;
    const int error_code =
        sqlite3_open_v2(m_filename.c_str(), &handle, connection_flags, nullptr);
    m_sqlite3_instance.reset(handle);

    if (OK != error_code) {
      throw Exception(handle, error_code);
    }

    if (busy_timeout_in_milliseconds > 0) {
      setBusyTimeout(busy_timeout_in_milliseconds);
    }
  }

  /// @brief Database connections cannot be copied.
  Database(const Database &) = delete;
  /// @brief Database connections cannot be copy-assigned.
  Database &operator=(const Database &) = delete;

  /// @brief Transfer ownership of a database connection.
  Database(Database &&aDatabase) = default;
  /// @brief Transfer ownership of a database connection by move assignment.
  Database &operator=(Database &&aDatabase) = default;

  /**
   * @brief Close the SQLite database connection when this object is destroyed.
   *
   * SQLite reports `SQLITE_BUSY` if outstanding statements or other resources
   * prevent the connection from closing. In that case the deleter asserts in
   * debug builds.
   */
  ~Database() = default;

  /// @brief Close the SQLite connection supplied to the deleter.
  struct Deleter {
    void operator()(::sqlite3 *sqlite3_instance) {
      const int error_code = sqlite3_close(
          sqlite3_instance); // Calling sqlite3_close() with a nullptr
                             // argument is a harmless no-op.

      // Avoid unreferenced variable warning when build in release mode
      (void)error_code;

      // Only case of error is SQLITE_BUSY: "database is locked" (some
      // statements are not finalized) Never throw an exception in a destructor
      assert(OK == error_code && "Database is locked");
    }
  };

  /**
   * @brief Return the raw SQLite database connection handle.
   *
   * Use this to call SQLite APIs directly or to integrate with libraries that
   * require a `sqlite3` handle. The returned pointer is non-owning and remains
   * valid only while this database connection is open.
   *
   * @return The underlying SQLite connection handle.
   */
  ::sqlite3 *getHandle() const noexcept { return m_sqlite3_instance.get(); }

  /**
   * @brief Set the maximum time SQLite waits when a database is busy.
   *
   * SQLite sleeps and retries until the timeout expires, after which an
   * operation may return `SQLITE_BUSY`. A non-positive timeout disables this
   * behavior. The current timeout can be read with `PRAGMA busy_timeout`.
   *
   * @param aBusyTimeoutMs Maximum time to wait, in milliseconds.
   *
   * @throws Exception if setting the timeout fails.
   */
  void setBusyTimeout(const int aBusyTimeoutMs) {
    const int error_code = sqlite3_busy_timeout(getHandle(), aBusyTimeoutMs);
    check(error_code);
  }

  /**
   * @brief Throw an exception if an SQLite result code is not `SQLITE_OK`.
   *
   * @param error_code SQLite result code to check.
   * @throws Exception when `error_code` is not `SQLITE_OK`.
   */
  void check(const int error_code) const {
    if (OK != error_code) {
      throw Exception(getHandle(), error_code);
    }
  }

protected:
private:
  std::unique_ptr<::sqlite3, Deleter> m_sqlite3_instance;
  std::string m_filename;
};
} // namespace finance::database::sqlite3