#pragma once

#include "finance/core/domain/currency.hpp"
#include "finance/core/repository/repository_interfaces.hpp"

namespace finance::core::repository {

/**
 * @brief Repository interface for finding currency records.
 *
 * Uses a currency code string to identify @ref domain::CurrencyInfo records.
 */
class CurrencyRepository
    : public IFindableRepository<std::string, domain::CurrencyInfo> {
public:
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  ~CurrencyRepository() override = default;
};
} // namespace finance::core::repository