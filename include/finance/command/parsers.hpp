#pragma once

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace finance::command {

/// @brief Describes a command-line argument and its handling callback.
struct ArgumentParser {
  /// Option keywords that identify this argument, such as `-s` or
  /// `--something`.
  const std::vector<std::string> keywords;

  /// Optional name of the value expected for this argument.
  const std::optional<std::string> argument_name;

  /// Short description of the argument, suitable for help or documentation.
  const std::string summary;

  /// Handles the remaining command-line arguments after a keyword is matched.
  ///
  /// The matched keyword is removed from the queue before the callback runs.
  const std::function<void(std::queue<std::string> &)> callback;
};

/// @brief Stores command metadata and the parsers for its
/// arguments/subcommands.
///
/// Argument keywords are dispatched to their associated callbacks. Nested
/// command parsers can be stored to represent subcommands.
struct CommandParser {
public:
  /// @brief Creates a parser with its metadata, arguments, and subcommands.
  /// @param name Command name recognized by this parser.
  /// @param summary Short description of the command.
  /// @param argument_parsers Argument keywords and callbacks supported by the
  /// command.
  /// @param subcommand_parsers Parsers for commands nested under this command.
  CommandParser(std::string name, std::string summary,
                CommandParser *parent_command_parser,
                std::vector<ArgumentParser> argument_parsers,
                std::vector<std::shared_ptr<CommandParser>> subcommand_parsers)
      : name(std::move(name)), summary(std::move(summary)),
        parent_command_parser(parent_command_parser),
        argument_parsers(std::move(argument_parsers)),
        subcommand_parsers(std::move(subcommand_parsers)) {}

  /// Command name recognized by this parser.
  const std::string name;

  /// Short description of the command, suitable for help or documentation.
  const std::string summary;

  /// Non-owning pointer to the parser that created this parser, or `nullptr`
  /// when this is a root parser.
  CommandParser *const parent_command_parser;

  /// Argument keywords and their callbacks.
  const std::vector<ArgumentParser> argument_parsers;

  /// Parsers for subcommands nested under this command.
  const std::vector<std::shared_ptr<CommandParser>> subcommand_parsers;

  /// @brief Processes queued tokens and dispatches matching argument keywords.
  ///
  /// Each matching keyword is removed from the queue before its callback is
  /// invoked. The callback receives the remaining queue. Once all tokens are
  /// processed, this command's execution hook is called.
  /// @param args Queue of command-line tokens to process.
  /// @return The command's result code, or 1 if a token is not recognized.
  int parse(std::queue<std::string> &args) const {
    // iterate over the remaining arguments in the queue by popping the nextitem
    while (!args.empty()) {
      // remove the next keyword
      const std::string keyword = args.front();
      args.pop();

      if (this->dispatchHelpDocumentation(keyword, args)) {
        return 0;
      }

      if (this->dispatchArgumentParsers(keyword, args)) {
        continue;
      }

      if (const std::optional<int> result =
              this->dispatchCommandParsers(keyword, args)) {
        return *result;
      }

      std::cerr << "Error: unrecognized command or option '" << keyword
                << "' for '" << name << "'.\n";
      return 1;
    }

    return executeCommand();
  };

protected:
  /// @brief Executes this command after its arguments have been parsed.
  ///
  /// Override this method in a derived parser to provide command behavior.
  /// @return 0 on success, or 1 when this command has no implementation.
  virtual int executeCommand() const {
    std::cerr << "Error: no action is defined for command '" << name << "'.\n";
    return 1;
  }

  /// @brief Prints this parser's help text and exits successfully.
  ///
  /// The output is generated from this parser's command metadata, arguments,
  /// and subcommands.
  virtual void printHelpDocumentation() const {
    std::vector<std::string> command_path;
    for (const CommandParser *parser = this; parser != nullptr;
         parser = parser->parent_command_parser) {
      command_path.push_back(parser->name);
    }
    std::reverse(command_path.begin(), command_path.end());

    std::vector<std::string> option_labels{"-h, --help"};
    std::size_t option_width = option_labels.front().size();
    for (const ArgumentParser &argument : argument_parsers) {
      std::string label;
      for (std::size_t i = 0; i < argument.keywords.size(); ++i) {
        if (i > 0) {
          label += ", ";
        }
        label += argument.keywords[i];
      }
      if (argument.argument_name) {
        label += " <";
        label += *argument.argument_name;
        label += ">";
      }
      if (label.size() > option_width) {
        option_width = label.size();
      }
      option_labels.push_back(std::move(label));
    }

    std::cout << "Usage:";
    for (const std::string &command_name : command_path) {
      std::cout << ' ' << command_name;
    }
    std::cout << " [OPTIONS]";
    if (!subcommand_parsers.empty()) {
      std::cout << " <COMMAND>";
    }
    std::cout << "\n\n" << summary << "\n\nOptions:\n";

    std::cout << "  " << std::left
              << std::setw(static_cast<int>(option_width + 2))
              << option_labels.front() << "Show this help message and exit\n";
    for (std::size_t i = 0; i < argument_parsers.size(); ++i) {
      std::cout << "  " << std::left
                << std::setw(static_cast<int>(option_width + 2))
                << option_labels[i + 1] << argument_parsers[i].summary << '\n';
    }

    if (!subcommand_parsers.empty()) {
      std::size_t command_width = 0;
      for (const std::shared_ptr<CommandParser> &parser : subcommand_parsers) {
        if (parser->name.size() > command_width) {
          command_width = parser->name.size();
        }
      }

      std::cout << "\nCommands:\n";
      for (const std::shared_ptr<CommandParser> &parser : subcommand_parsers) {
        std::cout << "  " << std::left
                  << std::setw(static_cast<int>(command_width + 2))
                  << parser->name << parser->summary << '\n';
      }
    }

    std::exit(EXIT_SUCCESS);
  }

private:
  bool dispatchHelpDocumentation(const std::string &keyword,
                                 std::queue<std::string> &args) const {
    if (keyword == "-h" || keyword == "--help") {
      this->printHelpDocumentation();
      return true;
    }
    return false;
  }

  /// @brief Dispatches a keyword to a matching argument callback.
  /// @param keyword Keyword to match against the registered arguments.
  /// @param args Remaining command-line tokens passed to the callback.
  /// @return `true` if a matching argument callback was invoked; otherwise
  /// `false`.
  bool dispatchArgumentParsers(const std::string &keyword,
                               std::queue<std::string> &args) const {
    // loop over the argument parsers
    for (const ArgumentParser &parser : argument_parsers) {
      // if we don't match on the keywords then move to the next argument
      // parser
      if (std::find(parser.keywords.begin(), parser.keywords.end(), keyword) !=
          parser.keywords.end()) {
        // call the parser's callback with the remaining arguments
        parser.callback(args);
        return true;
      }
    }

    // we found no argument matches
    return false;
  }

  /// @brief Dispatches a command name to its matching subcommand parser.
  /// @param keyword Command name to match against the registered subcommands.
  /// @param args Remaining command-line tokens passed to the subcommand parser.
  /// @return The subcommand's result code if matched; otherwise `std::nullopt`.
  std::optional<int>
  dispatchCommandParsers(const std::string &keyword,
                         std::queue<std::string> &args) const {

    for (const std::shared_ptr<CommandParser> &parser : subcommand_parsers) {
      if (keyword == parser->name) {
        return parser->parse(args);
      }
    }

    // we found no command matches
    return std::nullopt;
  }
};
} // namespace finance::command