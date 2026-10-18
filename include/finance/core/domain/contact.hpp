#pragma once

#include <string>

#include "finance/core/interfaces.hpp"

namespace finance::core::domain {
/**
 * @brief Strongly typed identifier for a contact.
 *
 * The underlying value is an unsigned integer.
 */
enum class ContactIdentifier : unsigned int {};

/**
 * @brief Describes a contact associated with a financial entity.
 */
struct Contact : Identifiable<ContactIdentifier> {
  /**
   * @brief Name of the contact.
   */
  std::string name;
};
} // namespace finance::core::domain