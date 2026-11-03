#pragma once

#include <iostream>
#include <string>

namespace finance::core::domain {

/// @brief  Describes the display and precision conventions of a currency.
struct Currency {
public:
  /// @brief
  static const char VERSION = 0;

  /// @brief
  static const unsigned char TWO_DECIMAL_PLACES = static_cast<unsigned char>(2);

  /// @brief
  Currency(std::string aCode, std::string aSymbol, unsigned char aDecimalPlaces)
      : code(aCode), symbol(aSymbol), decimal_places(aDecimalPlaces) {};

  /// @brief Restores a currency from its serialized representation.
  /// @param arInputStream Stream containing data in the format emitted by
  /// @ref operator<<. The version byte is consumed but not validated.
  explicit Currency(std::istream &arInputStream);

  /// @brief
  /// @param arOutputStream
  /// @param arBook
  /// @return
  friend std::ostream &operator<<(std::ostream &arOutputStream,
                                  const Currency &arCurrency);

  /// @brief Three-letter currency code, such as AUD, USD, or JPY.
  std::string code;

  /// @brief Number of fractional digits used to represent amounts in this
  /// currency. Defaults to 2.
  unsigned char decimal_places{2};

  /// @brief Symbol used to display this currency, such as $ or ¥.
  std::string symbol;
};
} // namespace finance::core::domain