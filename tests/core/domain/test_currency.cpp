#include <doctest/doctest.h>

#include <sstream>

#include "finance/core/domain/currency.hpp"

using finance::core::domain::Currency;

TEST_SUITE("core::domain::currency") {
  TEST_CASE("A currency can be serialized and restored without losing data") {
    const Currency expected{"KWD", "KD", 3};
    std::stringstream stream;

    stream << expected;
    stream.seekg(0);
    const Currency actual{stream};

    REQUIRE(actual.code == expected.code);
    REQUIRE(actual.symbol == expected.symbol);
    REQUIRE(actual.decimal_places == expected.decimal_places);
  }

  TEST_CASE("Each predefined currency can be serialized and restored") {
    const Currency *currencies[] = {
        &Currency::AUSTRALIAN_DOLLAR,
        &Currency::US_DOLLAR,
        &Currency::EURO,
        &Currency::POUND_STERLING,
        &Currency::JAPANESE_YEN,
        &Currency::CANADIAN_DOLLAR,
        &Currency::SWISS_FRANC,
        &Currency::CHINESE_YUAN_RENMINBI,
    };

    for (const Currency *expected : currencies) {
      INFO("Currency code: " << expected->code);
      std::stringstream stream;

      stream << *expected;
      stream.seekg(0);
      const Currency actual{stream};

      REQUIRE(actual.code == expected->code);
      REQUIRE(actual.symbol == expected->symbol);
      REQUIRE(actual.decimal_places == expected->decimal_places);
    }
  }
}
