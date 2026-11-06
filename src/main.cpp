
#include <queue>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "finance/command/global_command.hpp"

int main(int argc, char *argv[]) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

  // create a new argument queue
  std::queue<std::string> arguments;
  for (int i = 1; i < argc; ++i) {
    arguments.emplace(argv[i]);
  }

  // run the global command parser
  return finance::command::GLOBAL_COMMAND_PARSER.parse(arguments);
}