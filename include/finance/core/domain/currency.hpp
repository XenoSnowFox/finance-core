#pragma once

#include <iostream>
#include <string>

namespace finance::core::domain {

/**
 * @brief Currency metadata used to interpret and display monetary amounts.
 *
 * Stores a currency code, display symbol, and number of fractional digits.
 * Values are stored as supplied; construction does not validate them.
 */
struct Currency {
public:
  /**
   * @brief Version byte written at the start of serialized currency records.
   *
   * The deserialization constructor consumes this byte but does not verify it.
   */
  static const char VERSION = 0;

  /**
   * @brief Common precision for currencies with two fractional digits.
   */
  static const unsigned char TWO_DECIMAL_PLACES = static_cast<unsigned char>(2);

  /**
   * @brief Constructs currency metadata from its code, symbol, and precision.
   * @param aCode Currency code, stored without validation.
   * @param aSymbol Display symbol, stored without validation.
   * @param aDecimalPlaces Number of fractional digits, stored without validation.
   */
  Currency(std::string aCode, std::string aSymbol, unsigned char aDecimalPlaces)
      : code(aCode), symbol(aSymbol), decimal_places(aDecimalPlaces) {};

  /**
   * @brief Restores currency metadata from a serialized record.
   * @param arInputStream Input positioned at the start of a serialized record.
   *
   * The record contains a version byte, a 32-bit code length and the code
   * bytes, a precision byte, then a 32-bit symbol length and the symbol bytes.
   * Lengths are encoded in the host byte order.
   *
   * @note The version and record contents are not validated. The input must
   * contain a complete record in a compatible byte order.
   */
  explicit Currency(std::istream &arInputStream);

  /**
   * @brief Serializes currency metadata to an output stream.
   * @param arOutputStream Destination stream for the serialized record.
   * @param arCurrency Currency metadata to serialize.
   * @return The output stream, allowing chained writes.
   *
   * Writes the version byte, length-prefixed code, precision byte, and
   * length-prefixed symbol. Lengths are 32-bit unsigned integers in the host
   * byte order; strings longer than that length can represent are not
   * serialized faithfully.
   */
  friend std::ostream &operator<<(std::ostream &arOutputStream,
                                  const Currency &arCurrency);

  /**
   * @brief Currency code, commonly a three-letter code such as AUD, USD, or JPY.
   *
   * The value is not checked against a currency-code standard.
   */
  std::string code;

  /**
   * @brief Number of fractional digits used to represent amounts in this currency.
   *
   * Defaults to 2. The value is stored without range validation.
   */
  unsigned char decimal_places{2};

  /**
   * @brief Display symbol for this currency, such as $ or ¥.
   *
   * The value is used for display and is not validated.
   */
  std::string symbol;
};
} // namespace finance::core::domain