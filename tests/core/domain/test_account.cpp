#include <doctest/doctest.h>

#include <cstdint>
#include <sstream>
#include <string>

#include "finance/core/domain/account.hpp"

using finance::core::domain::Account;
using finance::core::domain::AccountType;

TEST_SUITE("core::domain::account") {
  TEST_CASE("An account can be serialized and restored") {
    const std::string identifier = "cash";
    const std::string currencyCode = "AUD";
    const std::string name = "Cash account";
    const AccountType type = AccountType::ASSET;

    std::stringstream input;
    input << Account::VERSION;

    const std::uint8_t identifierLength =
        static_cast<std::uint8_t>(identifier.size());
    input.write(reinterpret_cast<const char *>(&identifierLength),
                sizeof(identifierLength));
    input.write(identifier.data(), identifierLength);

    const std::uint8_t serializedType = static_cast<std::uint8_t>(type);
    input.write(reinterpret_cast<const char *>(&serializedType),
                sizeof(serializedType));

    const std::uint8_t currencyCodeLength =
        static_cast<std::uint8_t>(currencyCode.size());
    input.write(reinterpret_cast<const char *>(&currencyCodeLength),
                sizeof(currencyCodeLength));
    input.write(currencyCode.data(), currencyCodeLength);

    const std::uint16_t nameLength = static_cast<std::uint16_t>(name.size());
    input.write(reinterpret_cast<const char *>(&nameLength), sizeof(nameLength));
    input.write(name.data(), nameLength);

    input.seekg(0);
    const Account expected{input};

    std::stringstream output;
    output << expected;
    output.seekg(0);
    const Account actual{output};

    REQUIRE(actual.mIdentifier == identifier);
    REQUIRE(actual.mType == type);
    REQUIRE(actual.mCurrency.code == currencyCode);
    REQUIRE(actual.mCurrency.symbol.empty());
    REQUIRE(actual.mCurrency.decimal_places ==
            finance::core::domain::Currency::TWO_DECIMAL_PLACES);
    REQUIRE(actual.mName == name);
  }
}
