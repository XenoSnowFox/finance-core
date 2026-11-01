#pragma once

#include <string>

namespace finance::database {

/// @brief Names of the built-in database domains.
///
/// These identifiers are string values; this namespace does not validate
/// other domain names.
namespace domain {
/// @brief Identifier for the currency domain.
const std::string CURRENCY = "currency";

/// @brief Identifier for the system domain.
const std::string SYSTEM = "system";

}; // namespace domain

} // namespace finance::database