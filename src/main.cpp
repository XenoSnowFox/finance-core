
#include <queue>
#include <string>

#include "finance/command/global_command.hpp"

int main(int argc, char *argv[]) {

  // create a new argument queue
  std::queue<std::string> arguments;
  for (int i = 1; i < argc; ++i) {
    arguments.emplace(argv[i]);
  }

  // run the global command parser
  return finance::command::GLOBAL_COMMAND_PARSER.parse(arguments);
}