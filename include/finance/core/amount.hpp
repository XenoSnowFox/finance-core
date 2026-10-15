#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "finance/core/currency.hpp"

namespace finance::core {

/**
 * @brief Integer representation of an amount in millionths of a base unit.
 *
 * One base unit is represented by 1,000,000 micro-units.
 */
using MicroUnits = std::int64_t;

/**
 * @brief A monetary amount stored as an integer number of micro-units.
 *
 * This type avoids floating-point arithmetic for addition and subtraction.
 * Its representation is independent of currency precision; conversions to and
 * from minor units accept the currency's number of decimal places.
 */
class Amount {
public:
  /**
   * @brief Number of micro-units in one base unit.
   */
  static constexpr MicroUnits SCALE = 1'000'000;

  /**
   * @brief Constructs a zero amount.
   */
  constexpr Amount() noexcept = default;

  /**
   * @brief Constructs an amount from its raw micro-unit value.
   * @param micro Amount in micro-units.
   */
  [[nodiscard]] static constexpr Amount from_micro(MicroUnits micro) noexcept {
    return Amount(micro);
  }

  /**
   * @brief Constructs an amount from a value expressed in dollars.
   * @param dollars Value in dollars; fractional micro-units are truncated.
   */
  [[nodiscard]] static constexpr Amount from_dollars(double dollars) noexcept {
    return Amount(
        static_cast<MicroUnits>(dollars * static_cast<double>(SCALE)));
  }

  /**
   * @brief Constructs an amount from a value in currency minor units.
   *
   * The value is expressed in cents when @p decimal_places is 2; for other
   * precisions, it represents the corresponding minor unit.
   * @param cents Value in the currency's minor units.
   * @param decimal_places Number of fractional digits used by the currency;
   * must be between 0 and 6.
   * @throws std::out_of_range if @p decimal_places is outside that range.
   */
  [[nodiscard]] static constexpr Amount from_cents(std::int64_t cents,
                                                   int decimal_places) {
    return Amount(cents * micro_units_per_minor_unit(decimal_places));
  }

  /**
   * @brief Constructs an amount from currency minor units.
   *
   * The number of minor units is interpreted using the precision in
   * @p currencyInfo. For currencies with two decimal places, these are cents.
   * @param cents Value in the currency's minor units.
   * @param currencyInfo Currency metadata providing the number of decimal
   * places.
   */
  [[nodiscard]] static constexpr Amount
  from_cents(std::int64_t cents, CurrencyInfo &currencyInfo) noexcept {
    return from_cents(cents, currencyInfo.decimal_places);
  }

  /**
   * @brief Converts the amount to currency minor units, rounding to nearest.
   *
   * Half-unit values are rounded away from zero. The result is expressed in
   * cents when @p decimal_places is 2; for other precisions, it is expressed
   * in the corresponding minor unit.
   * @param decimal_places Number of fractional digits used by the currency;
   * must be between 0 and 6.
   * @throws std::out_of_range if @p decimal_places is outside that range.
   */
  [[nodiscard]] constexpr std::int64_t to_cents(int decimal_places) const {
    const MicroUnits factor = micro_units_per_minor_unit(decimal_places);
    return (m__micro_units >= 0) ? (m__micro_units + factor / 2) / factor
                                 : (m__micro_units - factor / 2) / factor;
  }

  /**
   * @brief Converts the amount to currency minor units, rounding to nearest.
   *
   * Half-unit values are rounded away from zero. The result is expressed
   * according to the precision in @p currencyInfo; when the currency has two
   * decimal places, the result is in cents.
   * @param currencyInfo Currency metadata providing the number of decimal
   * places.
   */
  [[nodiscard]] constexpr std::int64_t
  to_cents(CurrencyInfo &currencyInfo) const noexcept {
    return to_cents(currencyInfo.decimal_places);
  }

  /**
   * @brief Converts the amount to a dollar value.
   */
  [[nodiscard]] constexpr double to_dollars() const noexcept {
    return static_cast<double>(m__micro_units) / static_cast<double>(SCALE);
  }

  /**
   * @brief Returns the underlying value in micro-units.
   */
  [[nodiscard]] constexpr MicroUnits raw() const noexcept {
    return m__micro_units;
  }

  /**
   * @brief Returns the underlying value in micro-units.
   */
  [[nodiscard]] constexpr MicroUnits micro_units() const noexcept {
    return m__micro_units;
  }

  /**
   * @brief Adds another amount.
   */
  constexpr Amount operator+(const Amount &rhs) const noexcept {
    return Amount(m__micro_units + rhs.m__micro_units);
  }

  /**
   * @brief Subtracts another amount.
   */
  constexpr Amount operator-(const Amount &rhs) const noexcept {
    return Amount(m__micro_units - rhs.m__micro_units);
  }

  /**
   * @brief Returns the negated amount.
   */
  constexpr Amount operator-() const noexcept {
    return Amount(-m__micro_units);
  }

private:
  /**
   * @brief Returns the number of micro-units in one currency minor unit.
   * @param decimal_places Number of fractional digits used by the currency;
   * must be between 0 and 6.
   * @throws std::out_of_range if @p decimal_places is outside that range.
   */
  [[nodiscard]] static constexpr MicroUnits
  micro_units_per_minor_unit(int decimal_places) {
    if (decimal_places < 0 || decimal_places > 6) {
      throw std::out_of_range(
          "currency decimal places must be between 0 and 6");
    }
    MicroUnits factor = SCALE;
    for (int i = 0; i < decimal_places; ++i) {
      factor /= 10;
    }
    return factor;
  }

  /**
   * @brief Constructs an amount from its raw micro-unit value.
   * @param raw Amount in micro-units.
   */
  explicit constexpr Amount(MicroUnits raw) noexcept : m__micro_units(raw) {}

  /**
   * @brief Raw amount value in micro-units.
   */
  MicroUnits m__micro_units{0};
};

} // namespace finance::core