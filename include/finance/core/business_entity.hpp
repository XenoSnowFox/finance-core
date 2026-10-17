#pragma once

#include <string>

#include "finance/core/interfaces.hpp"

namespace finance::core {

/**
 * @brief Strongly typed identifier for a business entity.
 *
 * The underlying value is an unsigned integer.
 */
enum class BusinessEntityIdentifier : unsigned int {};

/**
 * @brief Represents a business entity, such as a company or organization.
 */
struct BusinessEntity : Identifiable<BusinessEntityIdentifier> {
  /**
   * @brief Name of the business entity.
   */
  std::string name;
};
} // namespace finance::core
