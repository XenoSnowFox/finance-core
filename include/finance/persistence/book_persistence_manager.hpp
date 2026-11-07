#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <system_error>
#include <utility>

#include "finance/core/domain/book.hpp"

namespace finance::persistence {

/**
 * @brief Provides callback-based access to a Book stored in a file.
 *
 * A transaction uses a fixed @c .bak backup path and does not coordinate
 * concurrent access from other managers or processes. A backup left by an
 * interrupted transaction must be recovered before this manager can be used.
 */
class BookPersistenceManager {
public:
  /**
   * @brief Constructs a manager for the specified Book file.
   * @param aFilename Path to the serialized Book file.
   */
  explicit BookPersistenceManager(std::string aFilename)
      : mFilename(std::move(aFilename)) {}

  /**
   * @brief Creates a new empty Book file without replacing an existing file.
   *
   * @throws std::filesystem::filesystem_error If the target already exists or
   * a filesystem operation fails.
   * @throws std::ios_base::failure If the new Book cannot be saved.
   * @return This manager, allowing chained operations.
   */
  BookPersistenceManager &create() {
    const std::filesystem::path filename(mFilename);
    if (std::filesystem::exists(filename)) {
      throw std::filesystem::filesystem_error(
          "Book file already exists", filename,
          std::make_error_code(std::errc::file_exists));
    }

    transaction([](core::domain::Book &) {});
    return *this;
  }

  /**
   * @brief Applies a callback to the loaded Book and saves it when the callback
   * returns successfully.
   * @param aCallback Operation to apply to the Book.
   *
   * Before saving, an existing file is renamed to the same path with a
   * @c .bak suffix. An existing backup prevents the transaction from starting.
   * If saving fails, the backup is restored.
   *
   * @throws std::filesystem::filesystem_error If a backup already exists or a
   * filesystem operation fails.
   * @throws std::ios_base::failure If an existing Book cannot be opened, read,
   * or saved.
   * @return This manager, allowing chained operations.
   */
  BookPersistenceManager &
  transaction(const std::function<void(core::domain::Book &)> &aCallback) {
    core::domain::Book book = loadBook();
    aCallback(book);

    const std::filesystem::path filename(mFilename);
    std::filesystem::path backupFilename(filename);
    backupFilename += ".bak";
    const bool hasExistingFile = std::filesystem::exists(filename);

    if (hasExistingFile) {
      if (std::filesystem::exists(backupFilename)) {
        throw std::filesystem::filesystem_error(
            "Book backup already exists", filename, backupFilename,
            std::make_error_code(std::errc::file_exists));
      }
      std::filesystem::rename(filename, backupFilename);
    }

    try {
      std::ofstream outputFile;
      outputFile.exceptions(std::ios::failbit | std::ios::badbit);
      outputFile.open(filename, std::ios::binary | std::ios::trunc);
      outputFile << book;
      outputFile.close();
    } catch (...) {
      std::filesystem::remove(filename);
      if (hasExistingFile) {
        std::filesystem::rename(backupFilename, filename);
      }
      throw;
    }

    if (hasExistingFile) {
      std::filesystem::remove(backupFilename);
    }
    return *this;
  }

  /**
   * @brief Invokes a callback with the loaded Book without saving changes.
   * @param aCallback Operation that reads the Book.
   *
   * If the file does not exist, the callback receives an empty Book.
   * @throws std::filesystem::filesystem_error If a backup is awaiting recovery.
   * @throws std::ios_base::failure If an existing Book cannot be opened or
   * read.
   * @return This manager, allowing chained operations.
   */
  BookPersistenceManager &readOnly(
      const std::function<void(const core::domain::Book &)> &aCallback) {
    const core::domain::Book book = loadBook();
    aCallback(book);
    return *this;
  }

private:
  /**
   * @brief Loads the Book from the configured file.
   * @return The deserialized Book, or an empty Book if the file is absent.
   *
   * Throws if an existing file cannot be opened or read.
   */
  core::domain::Book loadBook() const {
    std::filesystem::path backupFilename(mFilename);
    backupFilename += ".bak";
    if (std::filesystem::exists(backupFilename)) {
      throw std::filesystem::filesystem_error(
          "Book backup requires recovery", mFilename, backupFilename,
          std::make_error_code(std::errc::file_exists));
    }

    std::ifstream inputFile(mFilename, std::ios::binary);
    if (!inputFile) {
      if (std::filesystem::exists(mFilename)) {
        throw std::ios_base::failure("Unable to open Book file: " + mFilename);
      }
      return {};
    }

    core::domain::Book book(inputFile);
    if (inputFile.fail()) {
      throw std::ios_base::failure("Unable to read Book file: " + mFilename);
    }
    return book;
  }

  /**
   * @brief Path to the serialized Book file managed by this instance.
   */
  std::string mFilename;
};
} // namespace finance::persistence