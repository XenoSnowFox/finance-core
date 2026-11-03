/**
 * @file currency.cpp
 * @brief Implements serialization and restoration of Currency metadata.
 *
 * Serialized records contain a version byte, one-byte string lengths, raw
 * string bytes, and a precision byte. String lengths can represent at most
 * 255 bytes.
 */
#include "finance/core/domain/currency.hpp"

namespace finance::core::domain {
const Currency Currency::AUSTRALIAN_DOLLAR{"AUD", "$", 2};
const Currency Currency::US_DOLLAR{"USD", "$", 2};
const Currency Currency::EURO{"EUR", "€", 2};
const Currency Currency::POUND_STERLING{"GBP", "£", 2};
const Currency Currency::JAPANESE_YEN{"JPY", "¥", 0};
const Currency Currency::CANADIAN_DOLLAR{"CAD", "$", 2};
const Currency Currency::SWISS_FRANC{"CHF", "Fr", 2};
const Currency Currency::CHINESE_YUAN_RENMINBI{"CNY", "¥", 2};

Currency::Currency(std::istream &arInputStream) {
  // Consume the format version; compatibility is not checked.
  arInputStream.ignore(1);

  std::uint8_t length;
  // Read the one-byte code length and its bytes.
  arInputStream.read(reinterpret_cast<char *>(&length), sizeof(length));
  code.resize(length);
  if (length > 0) {
    arInputStream.read(code.data(), length);
  }

  // Read the precision byte.
  arInputStream.read(reinterpret_cast<char *>(&decimal_places),
                     sizeof(decimal_places));

  // Read the one-byte symbol length and its bytes.
  arInputStream.read(reinterpret_cast<char *>(&length), sizeof(length));
  symbol.resize(length);
  if (length > 0) {
    arInputStream.read(symbol.data(), length);
  }
}

std::ostream &operator<<(std::ostream &arOutputStream,
                         const Currency &arCurrency) {

  // Write the format version.
  arOutputStream << arCurrency.VERSION;

  // Write the one-byte code length and bytes.
  std::uint8_t length = static_cast<std::uint8_t>(arCurrency.code.size());
  arOutputStream.write(reinterpret_cast<const char *>(&length), sizeof(length));
  if (length > 0) {
    arOutputStream.write(arCurrency.code.data(), length);
  }

  // Write the precision byte.
  arOutputStream << arCurrency.decimal_places;

  // Write the one-byte symbol length and bytes.
  length = static_cast<std::uint8_t>(arCurrency.symbol.size());
  arOutputStream.write(reinterpret_cast<const char *>(&length), sizeof(length));
  if (length > 0) {
    arOutputStream.write(arCurrency.symbol.data(), length);
  }

  return arOutputStream;
}

} // namespace finance::core::domain