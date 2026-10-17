#pragma once

namespace finance::core {
/**
 * @brief Classification of an account in the chart of accounts.
 */
enum class AccountType { ASSET, LIABILITY, EQUITY, INCOME, EXPENSE };
} // namespace finance::core