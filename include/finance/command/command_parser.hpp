#pragma once

#include <string>

namespace finance::command {
/// Holds the name and summary used to describe a command parser.
struct CommandParser final {
  /// The parser's name.
  std::string name;

  /// A short summary describing the parser.
  std::string summary;
};
} // namespace finance::command