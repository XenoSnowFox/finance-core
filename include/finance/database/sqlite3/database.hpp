#pragma once

#include <cassert>
#include <memory>
#include <sqlite3.h>
#include <string>

#include "finance/database/database.hpp"
#include "finance/database/sqlite3/exception.hpp"

namespace finance::database::sqlite3 {
/// @brief SQLite's success result code.
extern const int OK = SQLITE_OK;

/// @brief Open an existing database in read-only mode.
extern const int OPEN_READONLY = SQLITE_OPEN_READONLY;

/// @brief Open an existing database for reading and writing, falling back to
/// read-only access if opening it for writing fails.
extern const int OPEN_READWRITE = SQLITE_OPEN_READWRITE;

/// @brief Create the database if it does not exist when used with
/// `OPEN_READWRITE`.
extern const int OPEN_CREATE = SQLITE_OPEN_CREATE;

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
  * `SQLITE_OPEN_URI` is included in `flags`.
  * @param flags SQLite open flags. Defaults to `OPEN_READONLY`.
  * @param busy_timeout_in_milliseconds Positive busy timeout to set after
  * opening, in milliseconds. Non-positive values leave SQLite's default
  * timeout unchanged.
  * @param apVfs Name of the SQLite VFS module to use, or `nullptr` for the
  * default VFS.
  * @throws Exception if SQLite cannot open the database or set the requested
  * busy timeout.
  */
 Database(const std::string &filename, const int flags = OPEN_READONLY,
          const int busy_timeout_in_milliseconds = 0,
          const char *apVfs = nullptr)
      : m_filename(filename) {
    ::sqlite3 *handle;
    const int error_code =
        sqlite3_open_v2(m_filename.c_str(), &handle, flags, apVfs);
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