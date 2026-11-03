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
 * Uses a currency code string to identify @ref domain::Currency records.
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
   * not validated.
   */
  void add(const domain::Currency &arCurrency) {
    const auto existingCurrency =
        std::find_if(mCurrencies.begin(), mCurrencies.end(),
                     [&arCurrency](const domain::Currency &arExistingCurrency) {
                       return arExistingCurrency.code == arCurrency.code;
                     });
    if (existingCurrency == mCurrencies.end()) {
      mCurrencies.push_back(arCurrency);
    } else {
      *existingCurrency = arCurrency;
    }
  }

  /**
   * @brief Calls a callback for each stored currency.
   * @param callback Function invoked with each currency as a const reference.
   *
   * Currencies are visited in insertion order. The callback cannot modify
   * repository records through its currency argument.
   */
  void forEach(
      const std::function<void(const domain::Currency &)> callback) const {
    for (const auto &currency : mCurrencies) {
      callback(currency);
    }
  }

private:
  std::vector<domain::Currency> mCurrencies;
};
} // namespace finance::core::repository