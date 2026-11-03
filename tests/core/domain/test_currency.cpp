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
}
