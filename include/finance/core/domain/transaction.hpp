#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "finance/core/domain/account.hpp"
#include "finance/core/domain/amount.hpp"
#include "finance/core/domain/contact.hpp"
#include "finance/core/interfaces.hpp"

namespace finance::core::domain {

/**
 * @brief Strongly typed identifier for a transaction.
 *
 * The underlying type is @c unsigned @c long @c long.
 */
enum class TransactionIdentifier : unsigned long long {};

/**
 * @brief Strongly typed identifier for a transaction entry.
 *
 * The underlying type is @c unsigned @c int.
 */
enum class TransactionEntryIdentifier : unsigned int {};

/**
 * @brief Represents an amount assigned to an account in a transaction.
 *
 * The foreign amount is recorded in the entry currency, while the base amount
 * is expressed in the ledger's base currency.
 */
struct TransactionEntry : Identifiable<TransactionEntryIdentifier> {
  /**
   * @brief Identifier of the transaction containing this entry.
   */
  TransactionIdentifier transaction_id;

  /**
   * @brief Identifier of the account to which this entry is assigned.
   */
  AccountIdentifier account_id;

  /**
   * @brief Optional identifier of the contact associated with this entry.
   *
   * An empty value indicates that no contact is associated with the entry.
   */
  std::optional<ContactIdentifier> contact_id;

  /**
   * @brief Currency in which @c foreign_amount is expressed.
   */
  CurrencyInfo currency;

  /**
   * @brief Entry amount expressed in @c currency.
   */
  Amount foreign_amount;

  /**
   * @brief Exchange rate represented in millionths.
   *
   * A value of @c Amount::SCALE represents an exchange rate of 1.000000.
   */
  MicroUnits exchange_rate_micro{Amount::SCALE};

  /**
   * @brief Entry amount expressed in the ledger's base currency.
   */
  Amount base_amount;
};

/**
 * @brief Represents a financial transaction and its entries.
 *
 * A transaction is balanced when the sum of its entries' base amounts is
 * zero. A valid transaction must contain at least two entries and be balanced.
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

  /**
   * @brief Checks whether the transaction's base amounts sum to zero.
   * @return @c true if the sum of all entry base amounts is zero; otherwise,
   * @c false.
   *
   * An empty transaction is considered balanced because its sum is zero.
   */
  [[nodiscard]] bool is_balanced() const noexcept {
    MicroUnits sum{0};
    for (const auto &entry : entries) {
      sum += entry.base_amount.raw();
    }
    return sum == 0;
  }

  /**
   * @brief Checks whether the transaction has at least two balanced entries.
   * @return @c true if the transaction contains at least two entries and is
   * balanced; otherwise, @c false.
   */
  [[nodiscard]] bool is_valid() const noexcept {
    return entries.size() >= 2 && is_balanced();
  }
};
} // namespace finance::core::domain