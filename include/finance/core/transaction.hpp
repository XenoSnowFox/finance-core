#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "finance/core/account.hpp"
#include "finance/core/amount.hpp"
#include "finance/core/contact.hpp"
#include "finance/core/interfaces.hpp"

namespace finance::core {

/**
 * @brief Strongly typed identifier for a transaction.
 *
 * The underlying type is @c unsigned @c long @c long.
 */
enum class TransactionIdentifier : unsigned long long {};

/**
 * @brief Strongly typed identifier for a transaction entry.
 *
 * The underlying value is an unsigned integer.
 */
enum class TransactionEntryIdentifier : unsigned int {};

/**
 * @brief Represents one account entry in a transaction.
 */
struct TransactionEntry : Identifiable<TransactionEntryIdentifier> {
  /**
   * @brief Identifier of the transaction this entry belongs to.
   */
  TransactionIdentifier transaction_id;

  /**
   * @brief Identifier of the account associated with this entry.
   */
  AccountIdentifier account_id;

  /**
   * @brief Optional identifier of the contact associated with this entry.
   */
  std::optional<ContactIdentifier> contact_id;

  /**
   * @brief Currency in which the foreign amount is expressed.
   */
  CurrencyInfo currency;

  /**
   * @brief Entry amount in the currency specified by @c currency.
   */
  Amount foreign_amount;

  /**
   * @brief Exchange rate in millionths, defaulting to 1.000000.
   */
  MicroUnits exchange_rate_micro{Amount::SCALE}; // Default 1.000000

  /**
   * @brief Entry amount expressed in the base currency.
   */
  Amount base_amount;
};

/**
 * @brief Represents a financial transaction and its entries.
 */
struct Transaction : Identifiable<TransactionIdentifier> {
  /**
   * @brief Short description of the transaction.
   */
  std::string description;

  /**
   * @brief Calendar date on which the transaction occurred.
   */
  std::chrono::year_month_day date;

  /**
   * @brief Additional notes associated with the transaction.
   */
  std::vector<std::string> notes;

  /**
   * @brief Account entries that make up the transaction.
   */
  std::vector<TransactionEntry> entries;
};
} // namespace finance::core