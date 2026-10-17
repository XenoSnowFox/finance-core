#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "finance/core/enumerators.hpp"
#include "finance/core/identifiers.hpp"
#include "finance/core/interfaces.hpp"

namespace finance::core {

/**
 * @brief Describes the display and precision conventions of a currency.
 */
struct CurrencyInfo {
  /**
   * @brief Three-letter currency code, such as AUD, USD, or JPY.
   */
  std::string_view code;

  /**
   * @brief Number of fractional digits used to represent amounts in this
   * currency.
   *
   * Defaults to 2.
   */
  int decimal_places{2};

  /**
   * @brief Symbol used to display this currency, such as $ or ¥.
   */
  std::string_view symbol;
};

/**
 * @brief Describes an account in the chart of accounts.
 *
 * An account has an identifier, a code and name, an accounting classification,
 * and the currency in which its amounts are recorded.
 */
struct Account : Identifiable<AccountIdentifier> {
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

/**
 * @brief Describes a contact associated with a financial entity.
 */
struct Contact : Identifiable<ContactIdentifier> {
  /**
   * @brief Name of the contact.
   */
  std::string name;
};

} // namespace finance::core