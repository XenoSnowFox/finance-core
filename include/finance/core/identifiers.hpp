#pragma once

namespace finance::core {

/**
 * @brief Strongly typed identifier for an account.
 *
 * The underlying value is an unsigned integer.
 */
enum class AccountIdentifier : unsigned int {};

/**
 * @brief Strongly typed identifier for a contact.
 *
 * The underlying value is an unsigned integer.
 */
enum class ContactIdentifier : unsigned int {};

} // namespace finance::core