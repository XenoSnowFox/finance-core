#include <doctest/doctest.h>

#include "finance/core/domain/transaction.hpp"

using finance::core::domain::Amount;
using finance::core::domain::Currency;
using finance::core::domain::Transaction;
using finance::core::domain::TransactionEntry;

TEST_SUITE("core::domain::transaction") {
  TEST_CASE("An empty transaction is balanced but not valid") {
    Transaction transaction;

    REQUIRE(transaction.is_balanced());
    REQUIRE_FALSE(transaction.is_valid());
  }

  TEST_CASE("A transaction with one balanced entry is not valid") {
    Transaction transaction;
    transaction.entries.emplace_back(Currency{"USD", "$", 2});

    REQUIRE(transaction.is_balanced());
    REQUIRE_FALSE(transaction.is_valid());
  }

  TEST_CASE(
      "A transaction with offsetting base amounts is balanced and valid") {
    Transaction transaction;
    TransactionEntry debit{Currency{"USD", "$", 2}};
    debit.base_amount = Amount::from_micro(250'000);
    TransactionEntry credit{Currency{"USD", "$", 2}};
    credit.base_amount = Amount::from_micro(-250'000);
    transaction.entries = {debit, credit};

    REQUIRE(transaction.is_balanced());
    REQUIRE(transaction.is_valid());
  }

  TEST_CASE(
      "A nonzero base-amount sum makes a transaction unbalanced and invalid") {
    Transaction transaction;
    TransactionEntry debit{Currency{"USD", "$", 2}};
    debit.base_amount = Amount::from_micro(250'000);
    TransactionEntry credit{Currency{"USD", "$", 2}};
    credit.base_amount = Amount::from_micro(-200'000);
    transaction.entries = {debit, credit};

    REQUIRE_FALSE(transaction.is_balanced());
    REQUIRE_FALSE(transaction.is_valid());
  }
}
