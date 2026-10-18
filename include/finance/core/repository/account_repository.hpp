#pragma once

#include "finance/core/domain/account.hpp"
#include "finance/core/repository/repository_interfaces.hpp"

namespace finance::core::repository {

/**
 * @brief Repository interface for finding account records.
 *
 * Uses @ref domain::AccountIdentifier to identify @ref domain::Account records.
 */
class AccountRepository
    : public IFindableRepository<domain::AccountIdentifier, domain::Account> {
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  virtual ~AccountRepository() = default;
};
} // namespace finance::core::repository