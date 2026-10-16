#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "finance/core/currency.hpp"

namespace finance::core {

/**
 * @brief Identifier type used to distinguish accounts.
 */
using AccountIdentifier = std::int64_t;

/**
 * @brief Classification of an account in the chart of accounts.
 */
enum class AccountType { ASSET, LIABILITY, EQUITY, INCOME, EXPENSE };

/**
 * @brief Describes an account in the chart of accounts.
 *
 * An account has an identifier, a code and name, an accounting classification,
 * and the currency in which its amounts are recorded.
 */
struct Account {
  /**
   * @brief Unique identifier for the account.
   */
  AccountIdentifier id;

  /**
   * @brief Code used to identify the account in the chart of accounts.
   */
  std::string code;

  /**
   * @brief Name of the account.
   */
  std::string name;

  /**
   * @brief Accounting classification of the account.
   */
  AccountType type;

  /**
   * @brief Currency used to record amounts for the account.
   */
  CurrencyInfo currency;
};

} // namespace finance::core