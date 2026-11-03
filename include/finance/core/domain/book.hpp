#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#include "finance/core/domain/currency.hpp"
#include "finance/core/repository/currency_repository.hpp"

namespace finance::core::domain {
class Book : public repository::CurrencyRepository {
public:
  static const std::uint8_t VERSION = 0;

  static const std::uint8_t RECORD_CURRENCY = 1;

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
