#include <catch2/catch_test_macros.hpp>

#include "finance/core/transaction.hpp"

using finance::core::Amount;
using finance::core::Transaction;
using finance::core::TransactionEntry;

TEST_CASE("An empty transaction is balanced but not valid") {
  Transaction transaction;

  REQUIRE(transaction.is_balanced());
  REQUIRE_FALSE(transaction.is_valid());
}

TEST_CASE("A transaction with one balanced entry is not valid") {
  Transaction transaction;
  transaction.entries.resize(1);

  REQUIRE(transaction.is_balanced());
  REQUIRE_FALSE(transaction.is_valid());
}

TEST_CASE("A transaction with offsetting base amounts is balanced and valid") {
  Transaction transaction;
  TransactionEntry debit;
  debit.base_amount = Amount::from_micro(250'000);
  TransactionEntry credit;
  credit.base_amount = Amount::from_micro(-250'000);
  transaction.entries = {debit, credit};

  REQUIRE(transaction.is_balanced());
  REQUIRE(transaction.is_valid());
}

TEST_CASE("A nonzero base-amount sum makes a transaction unbalanced and invalid") {
  Transaction transaction;
  TransactionEntry debit;
  debit.base_amount = Amount::from_micro(250'000);
  TransactionEntry credit;
  credit.base_amount = Amount::from_micro(-200'000);
  transaction.entries = {debit, credit};

  REQUIRE_FALSE(transaction.is_balanced());
  REQUIRE_FALSE(transaction.is_valid());
}
