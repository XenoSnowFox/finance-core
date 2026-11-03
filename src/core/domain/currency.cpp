/**
 * @file currency.cpp
 * @brief Implements serialization and restoration of Currency metadata.
 *
 * Serialized records use raw string bytes and 32-bit length fields encoded
 * using the host byte order.
 */
#include "finance/core/domain/currency.hpp"

namespace finance::core::domain {
Currency::Currency(std::istream &arInputStream) {
  // Consume the format version; compatibility is not checked.
  arInputStream.ignore(1);

  std::uint32_t length;
  // Read the code length and its bytes.
  arInputStream.read(reinterpret_cast<char *>(&length), sizeof(length));
  code.resize(length);
  if (length > 0) {
    arInputStream.read(code.data(), length);
  }

  // Read the precision byte.
  arInputStream.read(reinterpret_cast<char *>(&decimal_places),
                     sizeof(decimal_places));

  // Read the symbol length and its bytes.
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

  // Write the code length and bytes.
  std::uint32_t length = static_cast<std::uint32_t>(arCurrency.code.size());
  arOutputStream.write(reinterpret_cast<const char *>(&length), sizeof(length));
  if (length > 0) {
    arOutputStream.write(arCurrency.code.data(), length);
  }

  // Write the precision byte.
  arOutputStream << arCurrency.decimal_places;

  // Write the symbol length and bytes.
  length = static_cast<std::uint32_t>(arCurrency.symbol.size());
  arOutputStream.write(reinterpret_cast<const char *>(&length), sizeof(length));
  if (length > 0) {
    arOutputStream.write(arCurrency.symbol.data(), length);
  }

  return arOutputStream;
}

} // namespace finance::core::domain