#include "finance/core/domain/currency.hpp"

namespace finance::core::domain {
Currency::Currency(std::istream &arInputStream) {
  arInputStream.ignore(1);

  std::uint32_t length;
  arInputStream.read(reinterpret_cast<char *>(&length), sizeof(length));
  code.resize(length);
  if (length > 0) {
    arInputStream.read(code.data(), length);
  }

  arInputStream.read(reinterpret_cast<char *>(&decimal_places),
                     sizeof(decimal_places));

  arInputStream.read(reinterpret_cast<char *>(&length), sizeof(length));
  symbol.resize(length);
  if (length > 0) {
    arInputStream.read(symbol.data(), length);
  }
}

std::ostream &operator<<(std::ostream &arOutputStream,
                         const Currency &arCurrency) {

  // output the version number
  arOutputStream << arCurrency.VERSION;

  // output the three character code
  std::uint32_t length = static_cast<std::uint32_t>(arCurrency.code.size());
  arOutputStream.write(reinterpret_cast<const char *>(&length), sizeof(length));
  if (length > 0) {
    arOutputStream.write(arCurrency.code.data(), length);
  }

  // write the decimal places
  arOutputStream << arCurrency.decimal_places;

  // write the symbol
  length = static_cast<std::uint32_t>(arCurrency.symbol.size());
  arOutputStream.write(reinterpret_cast<const char *>(&length), sizeof(length));
  if (length > 0) {
    arOutputStream.write(arCurrency.symbol.data(), length);
  }

  return arOutputStream;
}

} // namespace finance::core::domain