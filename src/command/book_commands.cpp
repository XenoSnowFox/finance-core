#include "finance/command/book_commands.hpp"
#include "finance/command/global_command.hpp"
#include "finance/persistence/book_persistence_manager.hpp"

namespace finance::command {

int BookCreateCommandParser::executeCommand(
    const std::optional<std::string> &aPositionalArgument) const {
  GLOBAL_COMMAND_PARSER.book_filename =
      aPositionalArgument.value_or(GLOBAL_COMMAND_PARSER.book_filename);
  persistence::BookPersistenceManager{GLOBAL_COMMAND_PARSER.book_filename}
      .create();
  return 0;
}

} // namespace finance::command
