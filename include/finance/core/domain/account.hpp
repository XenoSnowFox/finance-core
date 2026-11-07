#pragma once

#include <cstdint>
#include <iostream>
#include <string>

#include "finance/core/domain/currency.hpp"
#include "finance/core/interfaces.hpp"

namespace finance::core::domain {

/**
 * @brief Classification of an account in the chart of accounts.
 *
 * This classification describes an account's role; it does not define
 * transaction-posting or balance-calculation behavior.
 */
enum class AccountType : std::uint8_t {
  /**
   * @brief The account's classification has not been determined.
   */
  UNKNOWN = 0,
  /**
   * @brief An economic resource controlled by the entity.
   */
  ASSET = 1,
  /**
   * @brief An obligation the entity owes to another party.
   */
  LIABILITY = 2,
  /**
   * @brief The residual interest in the entity's assets after liabilities.
   */
  EQUITY = 3,
  /**
   * @brief Value earned by the entity through its activities.
   */
  INCOME = 4,
  /**
   * @brief Cost incurred by the entity through its activities.
   */
  EXPENSE = 5,
};

/**
 * @brief Describes a ledger account.
 *
 * An account has an identifier, a name, an accounting classification, and one
 * currency for recording its amounts. Balances and transaction history are
 * not stored by this type.
 */
struct Account : Identifiable<std::string> {
  /**
   * @brief Version byte written at the start of serialized account records.
   *
   * The deserialization constructor consumes this byte but does not verify it.
   */
  static const std::uint8_t VERSION = 0;

  /**
   * @brief Creates an account with its identifier, classification, currency,
   * and name.
   * @param aIdentifier Identifier for this account, limited to 200 characters.
   * @param aType Accounting classification for this account.
   * @param arCurrency Currency used to record this account's amounts.
   * @param aName Human-readable name, limited to 500 characters.
   *
   * Throws std::length_error if the identifier or name exceeds its limit.
   */
  Account(std::string aIdentifier, AccountType aType, Currency &arCurrency,
          std::string aName);

  /**
   * @brief Restores account metadata from a serialized record.
   * @param arInputStream Input positioned at the start of an account record.
   *
   * The record stores the currency code but not its display symbol or
   * precision; those fields are initialized to an empty symbol and two
   * decimal places. The version byte is consumed but not checked.
   */
  explicit Account(std::istream &arInputStream);

  friend std::ostream &operator<<(std::ostream &arOutputStream,
                                  const Account &arAccount);

  /**
   * @brief Human-readable name of the account.
   *
   * This field does not enforce uniqueness or formatting.
   */
  std::string mName;

  /**
   * @brief Accounting classification of the account.
   *
   * The classification does not enforce transaction-posting rules or calculate
   * balances.
   */
  AccountType mType;

  /**
   * @brief Currency used to record amounts for this account.
   *
   * Each account stores one currency; currency conversion is not provided by
   * this field.
   */
  Currency mCurrency;
};

} // namespace finance::core::domain