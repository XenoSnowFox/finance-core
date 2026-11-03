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
      this->add(Currency(arInputStream));
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

  arBook.forEach([&arOutputStream, &arBook](const domain::Currency &currency) {
    arOutputStream << arBook.RECORD_CURRENCY << currency;
  });

  return arOutputStream;
}
} // namespace finance::core::domain