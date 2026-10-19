#pragma once

#include "finance/core/domain/transaction.hpp"
#include "finance/core/repository/repository_interfaces.hpp"

namespace finance::core::repository {

/**
 * @brief Repository interface for finding transaction records.
 *
 * Uses @ref domain::TransactionIdentifier to identify @ref domain::Transaction
 * records.
 */
class TransactionRepository
    : public IFindableRepository<domain::TransactionIdentifier,
                                 domain::Transaction> {
public:
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  ~TransactionRepository() override = default;
};
} // namespace finance::core::repository