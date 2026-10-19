#pragma once

#include "finance/core/domain/business_entity.hpp"
#include "finance/core/repository/repository_interfaces.hpp"

namespace finance::core::repository {

/**
 * @brief Repository interface for finding Business Entity records.
 *
 * Uses @ref domain::BusinessEntityIdentifier to identify @ref
 * domain::BusinessEntity records.
 */
class BusinessEntityRepository
    : public IFindableRepository<domain::BusinessEntityIdentifier,
                                 domain::BusinessEntity> {
public:
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  ~BusinessEntityRepository() override = default;
};
} // namespace finance::core::repository