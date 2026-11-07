#include "finance/command/account_commands.hpp"
#include "finance/command/global_command.hpp"
#include "finance/persistence/book_persistence_manager.hpp"
#include "finance/presentation/table_renderer.hpp"

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace finance::command {

namespace {

inline const finance::presentation::TableDefinition<core::domain::Account>
    AccountTableDefinition{
        .mDivider = "  ",
        .mColumns =
            {
                finance::presentation::TableColumn<core::domain::Account>{
                    .mHeading = "Code",
                    .mProcessor =
                        [](const core::domain::Account &account) {
                          return account.mIdentifier;
                        },
                },
                finance::presentation::TableColumn<core::domain::Account>{
                    .mHeading = "Type",
                    .mProcessor =
                        [](const core::domain::Account &account) {
                          using core::domain::AccountType;

                          switch (account.mType) {
                          case AccountType::UNKNOWN:
                            return "UNKNOWN";
                          case AccountType::ASSET:
                            return "ASSET";
                          case AccountType::LIABILITY:
                            return "LIABILITY";
                          case AccountType::EQUITY:
                            return "EQUITY";
                          case AccountType::INCOME:
                            return "INCOME";
                          case AccountType::EXPENSE:
                            return "EXPENSE";
                          }
                          return "UNKNOWN";
                        },
                },
                finance::presentation::TableColumn<core::domain::Account>{
                    .mHeading = "Currency",
                    .mAlignment = presentation::Alignment::MIDDLE,
                    .mProcessor =
                        [](const core::domain::Account &account) {
                          return account.mCurrency.code;
                        },
                },
                finance::presentation::TableColumn<core::domain::Account>{
                    .mHeading = "Name",
                    .mProcessor =
                        [](const core::domain::Account &account) {
                          return account.mName;
                        },
                },
            },
};

/**
 * @brief Maps a lowercase account type name to its classification.
 * @param aTypeName Supported type name: asset, liability, equity, income, or
 * expense.
 * @return The matching account type, or an empty optional if unsupported.
 */
std::optional<core::domain::AccountType>
accountTypeFromName(std::string_view aTypeName) {
  using core::domain::AccountType;

  if (aTypeName == "asset") {
    return AccountType::ASSET;
  }
  if (aTypeName == "liability") {
    return AccountType::LIABILITY;
  }
  if (aTypeName == "equity") {
    return AccountType::EQUITY;
  }
  if (aTypeName == "income") {
    return AccountType::INCOME;
  }
  if (aTypeName == "expense") {
    return AccountType::EXPENSE;
  }
  return std::nullopt;
}

/**
 * @brief Prints accounts in an aligned plain-text table with two spaces
 * between columns.
 * @param arAccounts Accounts to display, in the order supplied.
 *
 * Preserves input order. If the collection is empty, prints a message instead
 * of a table.
 */
void printAccountTable(const std::vector<core::domain::Account> &arAccounts) {
  if (arAccounts.empty()) {
    std::cout << "No account information found.";
    return;
  }

  presentation::TableRenderer<core::domain::Account> renderer{
      AccountTableDefinition};
  renderer.format(presentation::Format::TEXT);
  renderer.render(std::cout, arAccounts);
}

} // namespace

int CreateAccountByTypeCommandParser::executeCommand(
    const std::optional<std::string> &aPositionalArgument) const {
  if (mOptionError) {
    return 1;
  }

  if (!mCurrencyCode) {
    std::cerr << "Error: --currency is required.\n";
    return 1;
  }

  if (!mIdentifier) {
    std::cerr << "Error: --code is required.\n";
    return 1;
  }

  if (!aPositionalArgument) {
    std::cerr << "Error: account name is required.\n";
    return 1;
  }

  std::optional<core::domain::Currency> currency;
  bool accountAlreadyExists = false;
  persistence::BookPersistenceManager bookPersistenceManager{
      GLOBAL_COMMAND_PARSER.book_filename};
  bookPersistenceManager.readOnly([this, &currency, &accountAlreadyExists](
                                      const core::domain::Book &arBook) {
    arBook.all([this, &currency](
                   const std::vector<core::domain::Currency> &arCurrencies) {
      const auto matchingCurrency =
          std::find_if(arCurrencies.begin(), arCurrencies.end(),
                       [this](const core::domain::Currency &arCurrency) {
                         return arCurrency.code == *this->mCurrencyCode;
                       });
      if (matchingCurrency != arCurrencies.end()) {
        currency = *matchingCurrency;
      }
    });

    static_cast<const core::repository::AccountRepository &>(arBook).forEach(
        [this, &accountAlreadyExists](const core::domain::Account &arAccount) {
          accountAlreadyExists = accountAlreadyExists ||
                                 arAccount.mIdentifier == *this->mIdentifier;
        });
  });

  if (accountAlreadyExists) {
    std::cerr << "Error: account code '" << *mIdentifier
              << "' already exists.\n";
    return 1;
  }

  if (!currency) {
    std::cerr << "Error: currency code '" << *mCurrencyCode
              << "' was not found.\n";
    return 1;
  }

  const core::domain::Account account(*mIdentifier, mType, *currency,
                                      *aPositionalArgument);
  bookPersistenceManager.transaction(
      [&account](core::domain::Book &arBook) { arBook.add(account); });
  printAccountTable(std::vector<core::domain::Account>{account});
  return 0;
}

int ListAccountCommandParser::executeCommand(
    const std::optional<std::string> &aPositionalArgument) const {
  const std::optional<core::domain::AccountType> accountType =
      aPositionalArgument ? accountTypeFromName(*aPositionalArgument)
                          : std::nullopt;
  if (aPositionalArgument && !accountType) {
    std::cerr << "Error: unsupported account type '" << *aPositionalArgument
              << "'; expected asset, liability, equity, income, or expense.\n";
    return 1;
  }

  persistence::BookPersistenceManager{GLOBAL_COMMAND_PARSER.book_filename}
      .readOnly([&accountType](const core::domain::Book &arBook) {
        std::vector<core::domain::Account> accounts;
        static_cast<const core::repository::AccountRepository &>(arBook)
            .forEach([&accounts,
                      &accountType](const core::domain::Account &arAccount) {
              if (!accountType || arAccount.mType == *accountType) {
                accounts.push_back(arAccount);
              }
            });
        printAccountTable(accounts);
      });

  return 0;
}

} // namespace finance::command
