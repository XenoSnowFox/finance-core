/**
 * @file account.cpp
 * @brief Implements serialization and restoration of Account metadata.
 *
 * Serialized records contain a version byte, one-byte-length-prefixed
 * identifier and currency code, a type byte, and a two-byte-length-prefixed
 * name. Identifier and currency code lengths can represent at most 255 bytes;
 * the name length is stored in the host byte order.
 */
#include "finance/core/domain/account.hpp"

#include <stdexcept>
#include <utility>

namespace finance::core::domain {

Account::Account(std::string aIdentifier, AccountType aType,
                 Currency &arCurrency, std::string aName)
    : mName(std::move(aName)), mType(aType), mCurrency(arCurrency) {
  if (aIdentifier.size() > 200) {
    throw std::length_error("Account identifier cannot exceed 200 characters");
  }
  if (mName.size() > 500) {
    throw std::length_error("Account name cannot exceed 500 characters");
  }

  mIdentifier = std::move(aIdentifier);
}

Account::Account(std::istream &arInputStream)
    : mCurrency("", "", Currency::TWO_DECIMAL_PLACES) {
  // Consume the format version; compatibility is not checked.
  arInputStream.ignore(1);

  std::uint8_t length;

  // Read the one-byte identifier length and its bytes.
  arInputStream.read(reinterpret_cast<char *>(&length), sizeof(length));
  mIdentifier.resize(length);
  if (length > 0) {
    arInputStream.read(mIdentifier.data(), length);
  }

  // Read the account type.
  std::uint8_t type;
  arInputStream.read(reinterpret_cast<char *>(&type), sizeof(type));
  mType = static_cast<AccountType>(type);

  // Read the one-byte currency code length and its bytes.
  arInputStream.read(reinterpret_cast<char *>(&length), sizeof(length));
  mCurrency.code.resize(length);
  if (length > 0) {
    arInputStream.read(mCurrency.code.data(), length);
  }

  // Read the two-byte name length and its bytes.
  std::uint16_t nameLength;
  arInputStream.read(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));
  mName.resize(nameLength);
  if (nameLength > 0) {
    arInputStream.read(mName.data(), nameLength);
  }
}

std::ostream &operator<<(std::ostream &arOutputStream,
                         const Account &arAccount) {

  // Write the format version.
  arOutputStream << arAccount.VERSION;

  // Write the one-byte identifier length and bytes.
  {
    std::uint8_t length =
        static_cast<std::uint8_t>(arAccount.mIdentifier.size());
    arOutputStream.write(reinterpret_cast<const char *>(&length),
                         sizeof(length));
    if (length > 0) {
      arOutputStream.write(arAccount.mIdentifier.data(), length);
    }
  }

  // Write the type.
  const std::uint8_t type = static_cast<std::uint8_t>(arAccount.mType);
  arOutputStream.write(reinterpret_cast<const char *>(&type), sizeof(type));

  // Write the one-byte currency code length and bytes.
  {
    std::uint8_t length =
        static_cast<std::uint8_t>(arAccount.mCurrency.code.size());
    arOutputStream.write(reinterpret_cast<const char *>(&length),
                         sizeof(length));
    if (length > 0) {
      arOutputStream.write(arAccount.mCurrency.code.data(), length);
    }
  }

  // Write the two-byte name length and bytes.
  {
    std::uint16_t length = static_cast<std::uint16_t>(arAccount.mName.size());
    arOutputStream.write(reinterpret_cast<const char *>(&length),
                         sizeof(length));
    if (length > 0) {
      arOutputStream.write(arAccount.mName.data(), length);
    }
  }

  return arOutputStream;
}

} // namespace finance::core::domain