#include <filesystem>
#include <iostream>
#include <system_error>

#include "finance/command/database_commands.hpp"
#include "finance/command/global_command.hpp"
#include "finance/database/sqlite3/database.hpp"

namespace finance::command {

int CreateDatabaseCommandParser::executeCommand() const {
  const std::string &filename =
      this->positional_filename ? *this->positional_filename
                                : GLOBAL_COMMAND_PARSER.database_filename;

  std::error_code file_status_error;
  if (std::filesystem::exists(filename, file_status_error)) {
    std::cerr << "Error: database file '" << filename << "' already exists.\n";
    return 1;
  }
  if (file_status_error) {
    std::cerr << "Error: unable to check database file '" << filename
              << "': " << file_status_error.message() << '\n';
    return 1;
  }

  try {
    finance::database::sqlite3::Database{
        filename,
        {finance::database::sqlite3::ConnectionFlag::READ_WRITE,
         finance::database::sqlite3::ConnectionFlag::CREATE,
         finance::database::sqlite3::ConnectionFlag::EXCLUSIVE}};
  } catch (const finance::database::sqlite3::Exception &exception) {
    std::cerr << "Error: unable to create database file '" << filename
              << "': " << exception.what() << '\n';
    return 1;
  }

  std::cout << "Created database file '" << filename << "'.\n";
  return 0;
}

} // namespace finance::command
