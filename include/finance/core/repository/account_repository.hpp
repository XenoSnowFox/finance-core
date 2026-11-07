#pragma once

#include <functional>
#include <map>

#include "finance/core/domain/account.hpp"
#include "finance/core/repository/repository_interfaces.hpp"

namespace finance::core::repository {

/**
 * @brief Repository interface for finding account records.
 *
 * Uses @ref domain::AccountIdentifier to identify @ref domain::Account records.
 */
class AccountRepository {
public:
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  virtual ~AccountRepository() = default;

  /**
   * @brief Adds an account or replaces the account with the same identifier.
   * @param arAccount Account record to copy into the repository.
   *
   * Account identifiers are the repository keys; this operation does not
   * permit multiple records with the same identifier.
   */
  void add(const domain::Account &arAccount) {
    mAccounts.insert_or_assign(arAccount.mIdentifier, arAccount);
  }

  /**
   * @brief Calls a callback for each stored account.
   * @param aCallback Function invoked with each account as a const reference.
   *
   * Accounts are visited in case-sensitive alphabetical order by identifier.
   * The callback cannot modify repository records through its account argument.
   */
  void
  forEach(const std::function<void(const domain::Account &)> aCallback) const {
    for (const auto &[identifier, account] : mAccounts) {
      aCallback(account);
    }
  }

private:
  std::map<std::string, domain::Account> mAccounts;
};
} // namespace finance::core::repository