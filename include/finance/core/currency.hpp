#pragma once

#include <string_view>

namespace finance::core {
/**
 * @brief Describes the display and precision conventions of a currency.
 */
struct CurrencyInfo {
  /**
   * @brief Three-letter currency code, such as AUD, USD, or JPY.
   */
  std::string_view code;

  /**
   * @brief Number of fractional digits used to represent amounts in this
   * currency.
   *
   * Defaults to 2.
   */
  int decimal_places{2};

  /**
   * @brief Symbol used to display this currency, such as $ or ¥.
   */
  std::string_view symbol;
};
} // namespace finance::core