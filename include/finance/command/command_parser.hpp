#pragma once

#include <string>

namespace finance::command {
/// Holds the name and summary used to describe a command parser.
struct CommandParser {
  /// The parser's name.
  const std::string name;

  /// A short summary describing the parser.
  const std::string summary;
};
} // namespace finance::command