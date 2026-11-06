#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "finance/core/domain/currency.hpp"

namespace finance::core::repository {

/**
 * @brief Repository interface for finding currency records.
 *
 * Uses a currency code string to identify @ref domain::Currency records and
 * keeps them in case-sensitive alphabetical order by code.
 */
class CurrencyRepository {
public:
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  virtual ~CurrencyRepository() = default;

  /**
   * @brief Adds a currency or replaces the entry with the same code.
   * @param arCurrency Currency to add or replace.
   *
   * Matching uses an exact, case-sensitive code comparison. Currency data is
   * not validated. Records are kept in case-sensitive alphabetical order by
   * code.
   */
  void add(const domain::Currency &arCurrency) {
    const auto existingCurrency =
        std::find_if(mCurrencies.begin(), mCurrencies.end(),
                     [&arCurrency](const domain::Currency &arExistingCurrency) {
                       return arExistingCurrency.code == arCurrency.code;
                     });

    // matched an existing record, so update it
    if (existingCurrency != mCurrencies.end()) {
      *existingCurrency = arCurrency;
      return;
    }

    // new record, append and sort
    mCurrencies.push_back(arCurrency);
    std::sort(
        mCurrencies.begin(), mCurrencies.end(),
        [](const domain::Currency &aLeft, const domain::Currency &aRight) {
          return aLeft.code < aRight.code;
        });
  }

  /**
   * @brief Invokes a callback with all stored currencies.
   * @param aCallback Function that receives the currencies as a const vector.
   *
   * The vector is ordered case-sensitively by currency code. The callback
   * cannot modify repository records through its argument.
   */
  void all(const std::function<void(const std::vector<domain::Currency> &)>
               aCallback) const {
    aCallback(mCurrencies);
  }

  /**
   * @brief Calls a callback for each stored currency.
   * @param aCallback Function invoked with each currency as a const reference.
   *
   * Currencies are visited in case-sensitive alphabetical order by code. The
   * callback cannot modify repository records through its currency argument.
   */
  void
  forEach(const std::function<void(const domain::Currency &)> aCallback) const {
    for (const auto &currency : mCurrencies) {
      aCallback(currency);
    }
  }

private:
  /**
   * @brief Currency records maintained in case-sensitive alphabetical order
   * by code.
   */
  std::vector<domain::Currency> mCurrencies;
};
} // namespace finance::core::repository