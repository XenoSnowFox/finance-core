#pragma once

#include <iostream>

#include "finance/core/repository/account_repository.hpp"
#include "finance/core/repository/currency_repository.hpp"

namespace finance::core::domain {

/**
 * @brief Repository of currencies that can be serialized to and restored from
 * a stream.
 *
 * A serialized book contains a version byte followed by tagged currency
 * records, with no end marker. The reader does not validate the version and
 * terminates the process when it encounters an unsupported record tag.
 */
class Book : public repository::CurrencyRepository,
             public repository::AccountRepository {
public:
  using repository::AccountRepository::add;
  using repository::CurrencyRepository::add;

  /**
   * @brief Version byte written at the start of a serialized book.
   *
   * The reader consumes but does not validate this value.
   */
  static const std::uint8_t VERSION = 0;

  /**
   * @brief Record tag identifying a serialized currency record.
   */
  static const std::uint8_t RECORD_CURRENCY = 1;

  /**
   * @brief Record tag identifying a serialized account record.
   */
  static const std::uint8_t RECORD_ACCOUNT = 2;

  /**
   * @brief Creates an empty book.
   */
  Book() = default;

  /**
   * @brief Restores a book by reading tagged currency records from a stream.
   * @param arInputStream Stream positioned at the start of a serialized book.
   *
   * Reads records until the end of the stream. The version byte is consumed
   * but not checked; encountering an unsupported record tag terminates the
   * process.
   */
  explicit Book(std::istream &arInputStream);

  /**
   * @brief Serializes a book to an output stream.
   * @param arOutputStream Destination stream for the serialized book.
   * @param arBook Book whose currencies are serialized.
   * @return The output stream, allowing chained writes.
   *
   * Writes the version byte followed by each currency record tag and
   * serialized currency. No end marker is written.
   */
  friend std::ostream &operator<<(std::ostream &arOutputStream,
                                  const Book &arBook);
};
} // namespace finance::core::domain
