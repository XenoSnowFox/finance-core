#include "finance/core/domain/book.hpp"

namespace finance::core::domain {
Book::Book(std::istream &) : Book() {}

std::ostream &operator<<(std::ostream &arOutputStream, const Book &arBook) {

  // output the version number
  arOutputStream << arBook.VERSION;
  return arOutputStream;
}
} // namespace finance::core::domain