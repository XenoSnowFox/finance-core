#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <queue>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "finance/command/global_command.hpp"
#include "finance/persistence/book_persistence_manager.hpp"

namespace finance::command {

namespace {

/**
 * @brief Assigns an isolated temporary book file to the global command parser.
 *
 * Removes the temporary file and any transaction backup when the test ends.
 */
class ScopedBookFile {
public:
  /**
   * @brief Selects a unique temporary book file and remembers the old setting.
   */
  ScopedBookFile()
      : mPreviousFilename(GLOBAL_COMMAND_PARSER.book_filename),
        mFilename(
            (std::filesystem::temp_directory_path() /
             ("finance-core-currency-commands-" +
              std::to_string(std::chrono::steady_clock::now()
                                 .time_since_epoch()
                                 .count()) +
              ".book"))
                .string()) {
    GLOBAL_COMMAND_PARSER.book_filename = mFilename;
  }

  /**
   * @brief Restores the global filename and removes this test's book files.
   */
  ~ScopedBookFile() {
    std::error_code error;
    std::filesystem::remove(mFilename, error);
    std::filesystem::path backupFilename(mFilename);
    backupFilename += ".bak";
    std::filesystem::remove(backupFilename, error);
    GLOBAL_COMMAND_PARSER.book_filename = mPreviousFilename;
  }

  /**
   * @brief Returns the temporary book filename selected for this test.
   */
  const std::string &filename() const { return mFilename; }

private:
  /**
   * @brief Global book filename to restore when the test completes.
   */
  std::string mPreviousFilename;

  /**
   * @brief Unique temporary book filename used by the test.
   */
  std::string mFilename;
};

/**
 * @brief Restores an output stream's original buffer when the scope ends.
 */
class ScopedStreamRedirect {
public:
  /**
   * @brief Redirects a stream into the supplied output buffer.
   * @param arStream Stream whose output is redirected.
   * @param arOutput Buffer that receives the stream's output.
   */
  ScopedStreamRedirect(std::ostream &arStream, std::ostringstream &arOutput)
      : mrStream(arStream), mpOriginalBuffer(arStream.rdbuf(arOutput.rdbuf())) {
  }

  /**
   * @brief Restores the stream buffer that was active before redirection.
   */
  ~ScopedStreamRedirect() { mrStream.rdbuf(mpOriginalBuffer); }

private:
  /**
   * @brief Stream temporarily redirected by this object.
   */
  std::ostream &mrStream;

  /**
   * @brief Original stream buffer restored when this object is destroyed.
   */
  std::streambuf *mpOriginalBuffer;
};

/**
 * @brief Parses tokens using the process-wide command tree.
 * @param aArguments Command-line tokens to parse, in order.
 * @return The command parser's result code.
 */
int parseCommand(std::initializer_list<std::string> aArguments) {
  std::queue<std::string> args(aArguments);
  return GLOBAL_COMMAND_PARSER.parse(args);
}

} // namespace

TEST_SUITE("command::currency") {
  TEST_CASE("adding a built-in currency uppercases its code and uses defaults") {
    const ScopedBookFile bookFile;
    std::ostringstream output;
    const ScopedStreamRedirect outputRedirect(std::cout, output);

    CHECK(parseCommand({"currency", "add", "usd"}) == 0);

    std::vector<core::domain::Currency> currencies;
    persistence::BookPersistenceManager{bookFile.filename()}.readOnly(
        [&currencies](const core::domain::Book &arBook) {
          arBook.all([&currencies](
                         const std::vector<core::domain::Currency>
                             &arCurrencies) { currencies = arCurrencies; });
        });

    REQUIRE(currencies.size() == 1);
    CHECK(currencies[0].code == "USD");
    CHECK(currencies[0].symbol == "$");
    CHECK(currencies[0].decimal_places == 2);
    CHECK(output.str().find("USD") != std::string::npos);
  }

  TEST_CASE("adding a currency accepts a custom symbol and precision") {
    const ScopedBookFile bookFile;
    std::ostringstream output;
    const ScopedStreamRedirect outputRedirect(std::cout, output);

    CHECK(parseCommand(
              {"currency", "add", "xyz", "--symbol", "X", "--decimalPlaces",
               "3"}) == 0);

    persistence::BookPersistenceManager{bookFile.filename()}.readOnly(
        [](const core::domain::Book &arBook) {
          arBook.all([](const std::vector<core::domain::Currency>
                            &arCurrencies) {
            REQUIRE(arCurrencies.size() == 1);
            CHECK(arCurrencies[0].code == "XYZ");
            CHECK(arCurrencies[0].symbol == "X");
            CHECK(arCurrencies[0].decimal_places == 3);
          });
        });
  }

  TEST_CASE("adding a duplicate currency code is rejected") {
    const ScopedBookFile bookFile;
    std::ostringstream output;
    std::ostringstream error;
    const ScopedStreamRedirect outputRedirect(std::cout, output);
    const ScopedStreamRedirect errorRedirect(std::cerr, error);

    REQUIRE(parseCommand({"currency", "add", "USD"}) == 0);
    CHECK(parseCommand({"currency", "add", "USD"}) == 1);
    CHECK(error.str().find("already exists") != std::string::npos);

    persistence::BookPersistenceManager{bookFile.filename()}.readOnly(
        [](const core::domain::Book &arBook) {
          arBook.all([](const std::vector<core::domain::Currency>
                            &arCurrencies) { CHECK(arCurrencies.size() == 1); });
        });
  }

  TEST_CASE("adding a currency with an invalid code is rejected") {
    const ScopedBookFile bookFile;
    std::ostringstream error;
    const ScopedStreamRedirect errorRedirect(std::cerr, error);

    CHECK(parseCommand({"currency", "add", "US"}) == 1);
    CHECK(error.str().find("exactly three characters") != std::string::npos);
    CHECK_FALSE(std::filesystem::exists(bookFile.filename()));
  }

  TEST_CASE("adding a currency with an invalid precision is rejected") {
    const ScopedBookFile bookFile;
    std::ostringstream error;
    const ScopedStreamRedirect errorRedirect(std::cerr, error);

    CHECK(parseCommand(
              {"currency", "add", "XYZ", "--decimalPlaces", "11"}) == 1);
    CHECK(error.str().find("integer from 0 to 10") != std::string::npos);
    CHECK_FALSE(std::filesystem::exists(bookFile.filename()));
  }

  TEST_CASE("listing currencies reports an empty book") {
    const ScopedBookFile bookFile;
    std::ostringstream output;
    const ScopedStreamRedirect outputRedirect(std::cout, output);

    CHECK(parseCommand({"currency", "list"}) == 0);
    CHECK(output.str() == "No currency information found.");
  }

  TEST_CASE("listing currencies displays stored currencies in code order") {
    const ScopedBookFile bookFile;
    persistence::BookPersistenceManager{bookFile.filename()}.transaction(
        [](core::domain::Book &arBook) {
          arBook.add(core::domain::Currency{"USD", "$", 2});
          arBook.add(core::domain::Currency{"AUD", "$", 2});
        });

    std::ostringstream output;
    const ScopedStreamRedirect outputRedirect(std::cout, output);
    CHECK(parseCommand({"currency", "list"}) == 0);

    const std::string listing = output.str();
    const std::size_t australianDollarPosition = listing.find("AUD");
    const std::size_t usDollarPosition = listing.find("USD");
    REQUIRE(australianDollarPosition != std::string::npos);
    REQUIRE(usDollarPosition != std::string::npos);
    CHECK(australianDollarPosition < usDollarPosition);
    CHECK(listing.find("Decimal Places") != std::string::npos);
  }
}

} // namespace finance::command
