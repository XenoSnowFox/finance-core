#include "finance/core/domain/book.hpp"

namespace finance::core::domain {
Book::Book(std::istream &arInputStream) : Book() {
  // Consume the format version; compatibility is not checked.
  arInputStream.ignore(1);

  // The format has no end marker, so read record tags until end of stream.
  while (arInputStream.peek() != std::char_traits<char>::eof()) {
    std::uint8_t objectType;
    arInputStream.read(reinterpret_cast<char *>(&objectType),
                       sizeof(objectType));

    switch (objectType) {

    case Book::RECORD_CURRENCY:
      repository::CurrencyRepository::add(Currency(arInputStream));
      break;

    case Book::RECORD_ACCOUNT:
      repository::AccountRepository::add(Account(arInputStream));
      break;

    default:
      std::cerr << "UNABLE TO READ BOOK FILE.";
      exit(1);
    }
  }
}

std::ostream &operator<<(std::ostream &arOutputStream, const Book &arBook) {

  // output the version number
  arOutputStream << arBook.VERSION;

  // output all the currencies
  arBook.repository::CurrencyRepository::forEach(
      [&arOutputStream, &arBook](const domain::Currency &currency) {
        arOutputStream << arBook.RECORD_CURRENCY << currency;
      });

  // output all the accounts
  arBook.repository::AccountRepository::forEach(
      [&arOutputStream, &arBook](const domain::Account &arAccount) {
        arOutputStream << arBook.RECORD_ACCOUNT << arAccount;
      });

  return arOutputStream;
}
} // namespace finance::core::domain