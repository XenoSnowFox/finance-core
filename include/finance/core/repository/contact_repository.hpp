#pragma once

#include "finance/core/domain/contact.hpp"
#include "finance/core/repository/repository_interfaces.hpp"

namespace finance::core::repository {

/**
 * @brief Repository interface for finding contact records.
 *
 * Uses @ref domain::ContactIdentifier to identify @ref domain::Contact records.
 */
class ContactRepository
    : public IFindableRepository<domain::ContactIdentifier, domain::Contact> {
public:
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  ~ContactRepository() override = default;
};
} // namespace finance::core::repository