#include "finance/command/currency_commands.hpp"
#include "finance/command/global_command.hpp"
#include "finance/persistence/book_persistence_manager.hpp"
#include "finance/presentation/table_renderer.hpp"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace finance::command {

namespace {

inline const finance::presentation::TableDefinition<core::domain::Currency>
    CurrencyTableDefinition{
        .mDivider = "  ",
        .mColumns =
            {
                finance::presentation::TableColumn<core::domain::Currency>{
                    .mHeading = "Code",
                    .mProcessor =
                        [](const core::domain::Currency &arCurrency) {
                          return arCurrency.code;
                        },
                },
                finance::presentation::TableColumn<core::domain::Currency>{
                    .mHeading = "Symbol",
                    .mAlignment = presentation::Alignment::MIDDLE,
                    .mProcessor =
                        [](const core::domain::Currency &arCurrency) {
                          return arCurrency.symbol;
                        },
                },
                finance::presentation::TableColumn<core::domain::Currency>{
                    .mHeading = "Decimal Places",
                    .mAlignment = presentation::Alignment::MIDDLE,
                    .mProcessor =
                        [](const core::domain::Currency &arCurrency) {
                          return std::to_string(arCurrency.decimal_places);
                        },
                },
            },
};

/**
 * @brief Prints currencies in an aligned plain-text table with two spaces
 * between columns.
 * @param arCurrencies Currencies to display, in the order supplied.
 *
 * Preserves input order. If the collection is empty, prints a message
 * instead of a table.
 */
void printCurrencyTable(
    const std::vector<core::domain::Currency> &arCurrencies) {
  if (arCurrencies.empty()) {
    std::cout << "No currency information found.";
    return;
  }

  presentation::TableRenderer<core::domain::Currency> renderer{
      CurrencyTableDefinition};
  renderer.format(presentation::Format::TEXT);
  renderer.render(std::cout, arCurrencies);
}

} // namespace

int AddCurrencyCommandParser::executeCommand(
    const std::optional<std::string> &aPositionalArgument) const {
  if (mOptionError) {
    std::cerr << "Error: unable to add currency with code '"
              << aPositionalArgument.value_or("<missing>")
              << "': invalid option value.\n";
    return 1;
  }

  if (!aPositionalArgument || aPositionalArgument->size() != 3) {
    std::cerr << "Error: currency code must contain exactly three characters; "
                 "received '"
              << aPositionalArgument.value_or("<missing>") << "'.\n";
    return 1;
  }

  std::string currencyCode = *aPositionalArgument;
  std::transform(currencyCode.begin(), currencyCode.end(), currencyCode.begin(),
                 [](unsigned char aCharacter) {
                   return static_cast<char>(std::toupper(aCharacter));
                 });

  std::string symbol = mSymbol.value_or("$");
  std::uint8_t decimalPlaces = mDecimalPlaces.value_or(2);
  if (!mSymbol || !mDecimalPlaces) {
    const std::array<const core::domain::Currency *, 8> builtInCurrencies{
        &core::domain::Currency::AUSTRALIAN_DOLLAR,
        &core::domain::Currency::US_DOLLAR,
        &core::domain::Currency::EURO,
        &core::domain::Currency::POUND_STERLING,
        &core::domain::Currency::JAPANESE_YEN,
        &core::domain::Currency::CANADIAN_DOLLAR,
        &core::domain::Currency::SWISS_FRANC,
        &core::domain::Currency::CHINESE_YUAN_RENMINBI,
    };
    for (const core::domain::Currency *builtInCurrency : builtInCurrencies) {
      if (builtInCurrency->code == currencyCode) {
        if (!mSymbol) {
          symbol = builtInCurrency->symbol;
        }
        if (!mDecimalPlaces) {
          decimalPlaces = builtInCurrency->decimal_places;
        }
        break;
      }
    }
  }

  const core::domain::Currency currency(currencyCode, symbol, decimalPlaces);
  persistence::BookPersistenceManager bookPersistenceManager{
      GLOBAL_COMMAND_PARSER.book_filename};
  bool currencyAlreadyExists = false;
  bookPersistenceManager.readOnly([&currency, &currencyAlreadyExists](
                                      const core::domain::Book &arBook) {
    arBook.all([&currency, &currencyAlreadyExists](
                   const std::vector<core::domain::Currency> &arCurrencies) {
      currencyAlreadyExists = std::any_of(
          arCurrencies.begin(), arCurrencies.end(),
          [&currency](const core::domain::Currency &arExistingCurrency) {
            return arExistingCurrency.code == currency.code;
          });
    });
  });

  if (currencyAlreadyExists) {
    std::cerr << "Error: currency code '" << currency.code
              << "' already exists.\n";
    return 1;
  }

  bookPersistenceManager.transaction(
      [&currency](core::domain::Book &arBook) { arBook.add(currency); });
  printCurrencyTable(std::vector<core::domain::Currency>{currency});
  return 0;
}

int ListCurrencyCommandParser::executeCommand() const {
  persistence::BookPersistenceManager{GLOBAL_COMMAND_PARSER.book_filename}
      .readOnly([](const finance::core::domain::Book &arBook) {
        arBook.all([](const std::vector<finance::core::domain::Currency>
                          &arCurrencies) { printCurrencyTable(arCurrencies); });
      });

  return 0;
}

} // namespace finance::command
