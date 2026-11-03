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
  static const std::uint8_t VERSION = 0;

  /**
   * @brief Common precision for currencies with no fractional digits.
   */
  static const std::uint8_t ZERO_DECIMAL_PLACES = 0;

  /**
   * @brief Common precision for currencies with two fractional digits.
   */
  static const std::uint8_t TWO_DECIMAL_PLACES = 2;

  /**
   * @brief Common precision for currencies with three fractional digits.
   */
  static const std::uint8_t THREE_DECIMAL_PLACES = 3;

  /**
   * @brief Currency metadata for Australian dollars (AUD), using $ and 2
   * fractional digits.
   */
  static const Currency AUSTRALIAN_DOLLAR;

  /**
   * @brief Currency metadata for US dollars (USD), using $ and 2 fractional
   * digits.
   */
  static const Currency US_DOLLAR;

  /**
   * @brief Currency metadata for euros (EUR), using € and 2 fractional digits.
   */
  static const Currency EURO;

  /**
   * @brief Currency metadata for pound sterling (GBP), using £ and 2
   * fractional digits.
   */
  static const Currency POUND_STERLING;

  /**
   * @brief Currency metadata for Japanese yen (JPY), using ¥ and no
   * fractional digits.
   */
  static const Currency JAPANESE_YEN;

  /**
   * @brief Currency metadata for Canadian dollars (CAD), using $ and 2
   * fractional digits.
   */
  static const Currency CANADIAN_DOLLAR;

  /**
   * @brief Currency metadata for Swiss francs (CHF), using Fr and 2 fractional
   * digits.
   */
  static const Currency SWISS_FRANC;

  /**
   * @brief Currency metadata for Chinese yuan renminbi (CNY), using ¥ and 2
   * fractional digits.
   */
  static const Currency CHINESE_YUAN_RENMINBI;

  /**
   * @brief Constructs currency metadata from its code, symbol, and precision.
   * @param aCode Currency code, stored without validation.
   * @param aSymbol Display symbol, stored without validation.
   * @param aDecimalPlaces Number of fractional digits, stored without
   * validation.
   */
  Currency(std::string aCode, std::string aSymbol, unsigned char aDecimalPlaces)
      : code(aCode), symbol(aSymbol), decimal_places(aDecimalPlaces) {};

  /**
   * @brief Restores currency metadata from a serialized record.
   * @param arInputStream Input positioned at the start of a serialized record.
   *
   * The record contains a version byte, a one-byte code length and the code
   * bytes, a precision byte, then a one-byte symbol length and the symbol
   * bytes. Each length can represent at most 255 bytes.
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
   * Writes the version byte, one-byte-length-prefixed code, precision byte,
   * and one-byte-length-prefixed symbol. Each length can represent at most
   * 255 bytes; longer strings are not serialized faithfully.
   */
  friend std::ostream &operator<<(std::ostream &arOutputStream,
                                  const Currency &arCurrency);

  /**
   * @brief Currency code, commonly a three-letter code such as AUD, USD, or
   * JPY.
   *
   * The value is not checked against a currency-code standard.
   */
  std::string code;

  /**
   * @brief Number of fractional digits used to represent amounts in this
   * currency.
   *
   * Defaults to 2. The value is stored without range validation.
   */
  std::uint8_t decimal_places{2};

  /**
   * @brief Display symbol for this currency, such as $ or ¥.
   *
   * The value is used for display and is not validated.
   */
  std::string symbol;
};
} // namespace finance::core::domain