#pragma once

#include <iostream>
#include <sstream>

namespace finance::core::domain {
class Book {
public:
  static const char VERSION = 0;

  /**
   * @brief
   */
  Book() = default;

  /**
   * @brief
   * @param arInputStream
   */
  explicit Book(std::istream &arInputStream);

  friend std::ostream &operator<<(std::ostream &arOutputStream,
                                  const Book &arBook);
};
} // namespace finance::core::domain
