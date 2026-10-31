#pragma once

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include "finance/command/parsers.hpp"

namespace finance::command {

/// @brief Output formats supported by the documentation command.
enum OutputFormat {
  /// Plain-text documentation written to a `.txt` file.
  PLAIN_TEXT,
  /// Single-page HTML documentation.
  HTML,
  /// Markdown documentation.
  MARKDOWN,
  /// Unix manual-page documentation.
  MAN
};

/// @brief Parses `doc` options and writes documentation for the command tree.
///
/// The output includes registered commands, options, and examples, formatted
/// according to the selected output format.
struct DocumentationCommandParser : CommandParser {
  /// @brief Creates a documentation parser attached to its parent command.
  /// @param parent_command_parser Parser for the command that owns `doc`.
  explicit DocumentationCommandParser(CommandParser &parent_command_parser)
      : CommandParser{
            "doc",
            "Generate documentation for the tool.",
            &parent_command_parser,
            {ArgumentParser{
                 .keywords = {"--format"},
                 .argument_name = "format",
                 .summary = "Select the output format: html, md, man, or txt.",
                 .callback =
                     [this](std::queue<std::string> &args) {
                       if (args.empty()) {
                         std::cerr
                             << "Error: --format requires a format value.\n";
                         this->m_option_error = true;
                         return;
                       }

                       const std::string format = args.front();
                       args.pop();
                       if (format == "html") {
                         this->m_output_format = OutputFormat::HTML;
                       } else if (format == "md") {
                         this->m_output_format = OutputFormat::MARKDOWN;
                       } else if (format == "man") {
                         this->m_output_format = OutputFormat::MAN;
                       } else if (format == "txt") {
                         this->m_output_format = OutputFormat::PLAIN_TEXT;
                       } else {
                         std::cerr
                             << "Error: unsupported documentation format '"
                             << format << "'.\n";
                         this->m_option_error = true;
                       }
                     }},
             ArgumentParser{
                 .keywords = {"-o", "--output"},
                 .argument_name = "filename",
                 .summary = "Write documentation to this file. Defaults to "
                            "./docs/finance-core.txt "
                            "for text, ./docs/finance-core.html for HTML, "
                            "./docs/finance-core.md for Markdown, "
                            "and ./docs/finance-core.1 for man pages.",
                 .callback =
                     [this](std::queue<std::string> &args) {
                       if (args.empty()) {
                         std::cerr << "Error: --output requires a filename.\n";
                         this->m_option_error = true;
                         return;
                       }
                       this->m_output_filename = args.front();
                       args.pop();
                     }}},
            {},
            {CommandExample{"Generate a single-page HTML manual.",
                            "finance-core doc --format html --output "
                            "docs/finance-core.html"}}} {}

  /// Format selected by `--format`; defaults to plain text.
  OutputFormat m_output_format = OutputFormat::PLAIN_TEXT;

  /// Optional destination supplied through `-o` or `--output`.
  std::optional<std::string> m_output_filename = std::nullopt;

  /// @brief Writes the selected documentation format to its destination.
  /// @return `0` on success; `1` if option parsing or file output failed.
  int executeCommand() const override {
    if (m_option_error) {
      return 1;
    }

    std::string filename;
    switch (m_output_format) {
    case OutputFormat::PLAIN_TEXT:
      filename = m_output_filename.value_or("./docs/finance-core.txt");
      break;
    case OutputFormat::HTML:
      filename = m_output_filename.value_or("./docs/finance-core.html");
      break;
    case OutputFormat::MARKDOWN:
      filename = m_output_filename.value_or("./docs/finance-core.md");
      break;
    case OutputFormat::MAN:
      filename = m_output_filename.value_or("./docs/finance-core.1");
      break;
    }

    const std::filesystem::path output_path(filename);
    const std::filesystem::path output_directory = output_path.parent_path();
    if (!output_directory.empty()) {
      std::error_code error;
      std::filesystem::create_directories(output_directory, error);
      if (error) {
        std::cerr << "Error: could not create documentation output directory '"
                  << output_directory.string() << "': " << error.message()
                  << '\n';
        return 1;
      }
    }

    std::ofstream output(filename);
    if (!output) {
      std::cerr << "Error: could not open documentation output file '"
                << filename << "'.\n";
      return 1;
    }

    switch (m_output_format) {
    case OutputFormat::PLAIN_TEXT:
      writePlainTextDocumentation(output);
      break;
    case OutputFormat::HTML:
      writeHTMLDocumentation(output);
      break;
    case OutputFormat::MARKDOWN:
      writeMarkdownDocumentation(output);
      break;
    case OutputFormat::MAN:
      writeManDocumentation(output);
      break;
    }

    if (!output) {
      std::cerr << "Error: failed while writing documentation to '" << filename
                << "'.\n";
      return 1;
    }
    return 0;
  }

private:
  /// @brief Escapes text for safe inclusion in HTML.
  /// @param text Unescaped source text.
  /// @return The HTML-escaped text.
  static std::string htmlEscape(const std::string &text) {
    std::string result;
    for (const char character : text) {
      switch (character) {
      case '&':
        result += "&amp;";
        break;
      case '<':
        result += "&lt;";
        break;
      case '>':
        result += "&gt;";
        break;
      case '"':
        result += "&quot;";
        break;
      case '\'':
        result += "&#39;";
        break;
      default:
        result += character;
      }
    }
    return result;
  }

  /// @brief Gets the current local date formatted as a month and year.
  /// @return The formatted generation date.
  static std::string currentDate() {
    const std::time_t now = std::time(nullptr);
    std::tm date{};
#ifdef _WIN32
    localtime_s(&date, &now);
#else
    localtime_r(&now, &date);
#endif
    std::ostringstream result;
    result << std::put_time(&date, "%B %Y");
    return result.str();
  }

  /// @brief Builds the space-separated path from the root to a command.
  /// @param parser Command whose path is requested.
  /// @return The command path, including the root command name.
  static std::string commandPath(const CommandParser &parser) {
    std::vector<std::string> command_names;
    for (const CommandParser *current = &parser; current != nullptr;
         current = current->parent_command_parser) {
      command_names.push_back(current->name);
    }

    std::string path;
    for (auto name = command_names.rbegin(); name != command_names.rend();
         ++name) {
      if (!path.empty()) {
        path += ' ';
      }
      path += *name;
    }
    return path;
  }

  /// @brief Formats an option's keywords and optional argument name.
  /// @param argument Option metadata to format.
  /// @return A display label such as `--output <filename>`.
  static std::string argumentLabel(const ArgumentParser &argument) {
    std::string label;
    for (std::size_t index = 0; index < argument.keywords.size(); ++index) {
      if (index > 0) {
        label += ", ";
      }
      label += argument.keywords[index];
    }
    if (argument.argument_name) {
      label += " <";
      label += *argument.argument_name;
      label += ">";
    }
    return label;
  }

  /// @brief Collects every descendant command in traversal order.
  /// @param parser Root of the command subtree to traverse.
  /// @param commands Receives pointers to the collected commands.
  static void collectCommands(const CommandParser &parser,
                              std::vector<const CommandParser *> &commands) {
    for (const auto &subcommand : parser.subcommand_parsers) {
      commands.push_back(subcommand.get());
      collectCommands(*subcommand, commands);
    }
  }

  /// @brief Collects examples from a command and all its descendants.
  /// @param parser Root of the command subtree to traverse.
  /// @param examples Receives pointers to the collected examples.
  static void collectExamples(const CommandParser &parser,
                              std::vector<const CommandExample *> &examples) {
    for (const CommandExample &example : parser.examples) {
      examples.push_back(&example);
    }
    for (const auto &subcommand : parser.subcommand_parsers) {
      collectExamples(*subcommand, examples);
    }
  }

  /// @brief Gets the project version configured by CMake.
  /// @return The version prefixed with `v`.
  static std::string currentVersion() { return "v" FINANCE_CORE_VERSION; }

  /// @brief Writes the complete HTML manual-page document.
  /// @param output Stream receiving the generated document.
  void writeHTMLDocumentation(std::ostream &output) const {
    const std::string page_name = htmlEscape(parent_command_parser->name);
    const std::string man_name = htmlEscape(parent_command_parser->name);
    const std::string page_summary = htmlEscape(parent_command_parser->summary);
    const std::string date = htmlEscape(currentDate());
    const std::string version = htmlEscape(currentVersion());

    output << R"(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>)" << page_name
           << R"( — Manual Page</title>
  <style>
    :root {
      --bg: #ffffff;
      --text: #1f2328;
      --code-bg: #f6f8fa;
      --border: #d0d7de;
      --accent: #0969da;
      --font-mono: ui-monospace, SFMono-Regular, "SF Mono", Consolas, "Liberation Mono", Menlo, monospace;
    }

    @media (prefers-color-scheme: dark) {
      :root {
        --bg: #0d1117;
        --text: #e6edf3;
        --code-bg: #161b22;
        --border: #30363d;
        --accent: #2f81f7;
      }
    }

    body {
      background-color: var(--bg);
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      line-height: 1.6;
      max-width: 860px;
      margin: 0 auto;
      padding: 2rem 1rem;
    }

    header {
      display: flex;
      justify-content: space-between;
      border-bottom: 2px solid var(--border);
      padding-bottom: 0.5rem;
      margin-bottom: 2rem;
      font-family: var(--font-mono);
      font-size: 0.9rem;
      text-transform: uppercase;
    }

    h1, h2 {
      border-bottom: 1px solid var(--border);
      padding-bottom: 0.3rem;
      margin-top: 2rem;
    }

    h2 {
      font-size: 1.25rem;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    dl {
      margin-left: 1rem;
    }

    dt {
      font-family: var(--font-mono);
      font-weight: bold;
      margin-top: 1rem;
      color: var(--accent);
    }

    dd {
      margin-left: 1.5rem;
      margin-top: 0.25rem;
    }

    dd dl {
      margin-left: 0;
    }

    code, pre {
      font-family: var(--font-mono);
      background-color: var(--code-bg);
      border-radius: 6px;
    }

    code {
      padding: 0.2em 0.4em;
      font-size: 85%;
    }

    pre {
      padding: 1rem;
      overflow-x: auto;
      border: 1px solid var(--border);
    }

    .synopsis code {
      display: block;
      padding: 0.5rem 1rem;
      margin-bottom: 0.5rem;
      background: var(--code-bg);
    }

    footer {
      margin-top: 4rem;
      border-top: 1px solid var(--border);
      padding-top: 1rem;
      display: flex;
      justify-content: space-between;
      font-family: var(--font-mono);
      font-size: 0.85rem;
      color: #656d76;
    }
  </style>
</head>
<body>

  <header>
    <span>)"
           << man_name << R"((1)</span>
    <span>User Commands</span>
    <span>)"
           << man_name << R"((1)</span>
  </header>

  <main>
    <section id="name">
      <h2>NAME</h2>
      <p><strong>)"
           << page_name << "</strong> &mdash; " << page_summary << R"(</p>
    </section>

    <section id="synopsis">
      <h2>SYNOPSIS</h2>
      <div class="synopsis">
)";
    writeHTMLSynopsis(output, *parent_command_parser);
    output << R"(      </div>
    </section>

    <section id="description">
      <h2>DESCRIPTION</h2>
      <p>)" << page_summary
           << R"(</p>
    </section>

    <section id="commands">
      <h2>COMMANDS</h2>
)";
    writeHTMLCommands(output, *parent_command_parser);
    output << R"(    </section>

    <section id="global-options">
      <h2>GLOBAL OPTIONS</h2>
)";
    writeHTMLGlobalOptions(output, *parent_command_parser);
    output << R"(    </section>

    <section id="examples">
      <h2>EXAMPLES</h2>
)";
    writeHTMLExamples(output, *parent_command_parser);
    output << R"(    </section>
  </main>

  <footer>
    <span>)"
           << version << "</span>\n    <span>" << date << "</span>\n"
           << "    <span>" << man_name << R"((1)</span>
  </footer>

</body>
</html>
)";
  }

  /// @brief Writes synopsis entries for the root and every registered command.
  /// @param output Stream receiving the synopsis markup.
  /// @param root Root command parser.
  static void writeHTMLSynopsis(std::ostream &output,
                                const CommandParser &root) {
    const std::string root_name = htmlEscape(root.name);
    output << "        <code><strong>" << root_name
           << "</strong> [<em>GLOBAL_OPTIONS</em>] <em>COMMAND</em> "
              "[<em>ARGS</em>...]</code>\n";

    std::vector<const CommandParser *> commands;
    collectCommands(root, commands);
    for (const CommandParser *command : commands) {
      output << "        <code><strong>" << htmlEscape(commandPath(*command))
             << "</strong>";
      if (!command->argument_parsers.empty()) {
        output << " [<em>OPTIONS</em>]";
      }
      if (!command->subcommand_parsers.empty()) {
        output << " <em>SUBCOMMAND</em> [<em>ARGS</em>...]";
      }
      output << "</code>\n";
    }
  }

  /// @brief Writes one command and its options and nested subcommands as HTML.
  /// @param output Stream receiving the command markup.
  /// @param command Command parser to render.
  static void writeHTMLCommand(std::ostream &output,
                               const CommandParser &command) {
    output << "        <dt>" << htmlEscape(command.name);
    if (!command.subcommand_parsers.empty()) {
      output << " <em>SUBCOMMAND</em>";
    }
    output << "</dt>\n        <dd>" << htmlEscape(command.summary);
    if (!command.argument_parsers.empty() ||
        !command.subcommand_parsers.empty()) {
      output << "\n          <dl>\n";
      for (const ArgumentParser &argument : command.argument_parsers) {
        output << "            <dt>" << htmlEscape(argumentLabel(argument))
               << "</dt>\n            <dd>" << htmlEscape(argument.summary)
               << "</dd>\n";
      }
      for (const auto &subcommand : command.subcommand_parsers) {
        writeHTMLCommand(output, *subcommand);
      }
      output << "          </dl>";
    }
    output << "</dd>\n";
  }

  /// @brief Writes the root command's full command hierarchy as HTML.
  /// @param output Stream receiving the command markup.
  /// @param root Root command parser.
  static void writeHTMLCommands(std::ostream &output,
                                const CommandParser &root) {
    output << "      <dl>\n";
    for (const auto &command : root.subcommand_parsers) {
      writeHTMLCommand(output, *command);
    }
    output << "      </dl>\n";
  }

  /// @brief Writes help and root-level options as HTML.
  /// @param output Stream receiving the options markup.
  /// @param root Root command parser.
  static void writeHTMLGlobalOptions(std::ostream &output,
                                     const CommandParser &root) {
    output << "      <dl>\n"
              "        <dt>-h, --help</dt>\n"
              "        <dd>Display summary help and exit.</dd>\n";
    for (const ArgumentParser &argument : root.argument_parsers) {
      output << "        <dt>" << htmlEscape(argumentLabel(argument))
             << "</dt>\n        <dd>" << htmlEscape(argument.summary)
             << "</dd>\n";
    }
    output << "      </dl>\n";
  }

  /// @brief Writes examples from the root command tree as HTML.
  /// @param output Stream receiving the examples markup.
  /// @param root Root command parser.
  static void writeHTMLExamples(std::ostream &output,
                                const CommandParser &root) {
    std::vector<const CommandExample *> examples;
    collectExamples(root, examples);
    for (const CommandExample *example : examples) {
      output << "      <p>" << htmlEscape(example->description)
             << "</p>\n      <pre><code>" << htmlEscape(example->command)
             << "</code></pre>\n";
    }
  }

  /// @brief Writes the documentation as a Markdown manual page.
  /// @param output Stream receiving the generated document.
  void writeMarkdownDocumentation(std::ostream &output) const {
    const CommandParser &root = *parent_command_parser;
    output << "# " << root.name << "(1) — User Commands\n\n";
    output << "## NAME\n\n**" << root.name << "** — " << root.summary << "\n\n";
    output << "## SYNOPSIS\n\n```text\n";
    writePlainSynopsis(output, root);
    output << "```\n\n## DESCRIPTION\n\n"
           << root.summary << "\n\n## COMMANDS\n\n";
    writeMarkdownCommands(output, root);
    output << "\n## GLOBAL OPTIONS\n\n- `-h`, `--help` — Display summary help "
              "and exit.\n";
    for (const ArgumentParser &argument : root.argument_parsers) {
      output << "- `" << argumentLabel(argument) << "` — " << argument.summary
             << '\n';
    }
    output << "\n## EXAMPLES\n\n";
    writeMarkdownExamples(output, root);
    output << "\n---\n\n"
           << currentVersion() << "  \n"
           << currentDate() << "  \n"
           << root.name << "(1)\n";
  }

  /// @brief Writes synopsis entries for the root and all descendant commands.
  /// @param output Stream receiving the synopsis text.
  /// @param root Root command parser.
  /// @param indentation Prefix applied to each synopsis line.
  static void writePlainSynopsis(std::ostream &output,
                                 const CommandParser &root,
                                 const std::string &indentation = "") {
    output << indentation << root.name
           << " [GLOBAL_OPTIONS] COMMAND [ARGS...]\n";
    std::vector<const CommandParser *> commands;
    collectCommands(root, commands);
    for (const CommandParser *command : commands) {
      output << indentation << commandPath(*command);
      if (!command->argument_parsers.empty()) {
        output << " [OPTIONS]";
      }
      if (!command->subcommand_parsers.empty()) {
        output << " SUBCOMMAND [ARGS...]";
      }
      output << '\n';
    }
  }

  /// @brief Writes a command and its options and subcommands as Markdown.
  /// @param output Stream receiving the command list.
  /// @param command Command parser to render.
  /// @param depth Nesting depth used to indent the list.
  static void writeMarkdownCommand(std::ostream &output,
                                   const CommandParser &command,
                                   std::size_t depth) {
    output << std::string(depth * 2, ' ') << "- **" << command.name;
    if (!command.subcommand_parsers.empty()) {
      output << " SUBCOMMAND";
    }
    output << "** — " << command.summary << '\n';
    for (const ArgumentParser &argument : command.argument_parsers) {
      output << std::string((depth + 1) * 2, ' ') << "- `"
             << argumentLabel(argument) << "` — " << argument.summary << '\n';
    }
    for (const auto &subcommand : command.subcommand_parsers) {
      writeMarkdownCommand(output, *subcommand, depth + 1);
    }
  }

  /// @brief Writes the root command's full command hierarchy as Markdown.
  /// @param output Stream receiving the command list.
  /// @param root Root command parser.
  static void writeMarkdownCommands(std::ostream &output,
                                    const CommandParser &root) {
    for (const auto &command : root.subcommand_parsers) {
      writeMarkdownCommand(output, *command, 0);
    }
  }

  /// @brief Writes examples from the root command tree as Markdown.
  /// @param output Stream receiving the examples.
  /// @param root Root command parser.
  static void writeMarkdownExamples(std::ostream &output,
                                    const CommandParser &root) {
    std::vector<const CommandExample *> examples;
    collectExamples(root, examples);
    for (const CommandExample *example : examples) {
      output << example->description << "\n\n```sh\n"
             << example->command << "\n```\n\n";
    }
  }

  /// @brief Writes the documentation as plain text.
  /// @param output Stream receiving the generated document.
  void writePlainTextDocumentation(std::ostream &output) const {
    const CommandParser &root = *parent_command_parser;
    output << root.name << "(1) — USER COMMANDS\n\n";
    output << "NAME\n    " << root.name << " — " << root.summary << "\n\n";
    output << "SYNOPSIS\n";
    writePlainSynopsis(output, root, "    ");
    output << "\nDESCRIPTION\n    " << root.summary << "\n\nCOMMANDS\n";
    writePlainCommands(output, root, 1);
    output << "\nGLOBAL OPTIONS\n";
    writePlainOption(output, "-h, --help", "Display summary help and exit.", 1);
    for (const ArgumentParser &argument : root.argument_parsers) {
      writePlainOption(output, argumentLabel(argument), argument.summary, 1);
    }
    output << "\nEXAMPLES\n";
    writePlainExamples(output, root);
    output << '\n'
           << currentVersion() << " | " << currentDate() << " | " << root.name
           << "(1)\n";
  }

  /// @brief Writes an indented option label and its description.
  /// @param output Stream receiving the option entry.
  /// @param label Display label for the option.
  /// @param summary Description of the option.
  /// @param indentation Indentation depth for the entry.
  static void writePlainOption(std::ostream &output, const std::string &label,
                               const std::string &summary,
                               std::size_t indentation) {
    output << std::string(indentation * 2, ' ') << label << "\n"
           << std::string((indentation + 1) * 2, ' ') << summary << '\n';
  }

  /// @brief Writes commands, options, and nested subcommands as plain text.
  /// @param output Stream receiving the command tree.
  /// @param parser Root of the command subtree to render.
  /// @param indentation Indentation depth for the subtree.
  static void writePlainCommands(std::ostream &output,
                                 const CommandParser &parser,
                                 std::size_t indentation) {
    for (const auto &command : parser.subcommand_parsers) {
      output << std::string(indentation * 2, ' ') << command->name;
      if (!command->subcommand_parsers.empty()) {
        output << " SUBCOMMAND";
      }
      output << "\n"
             << std::string((indentation + 1) * 2, ' ') << command->summary
             << '\n';
      for (const ArgumentParser &argument : command->argument_parsers) {
        writePlainOption(output, argumentLabel(argument), argument.summary,
                         indentation + 1);
      }
      writePlainCommands(output, *command, indentation + 1);
    }
  }

  /// @brief Writes examples from the root command tree as plain text.
  /// @param output Stream receiving the examples.
  /// @param root Root command parser.
  static void writePlainExamples(std::ostream &output,
                                 const CommandParser &root) {
    std::vector<const CommandExample *> examples;
    collectExamples(root, examples);
    for (const CommandExample *example : examples) {
      output << "    " << example->description << "\n"
             << "        " << example->command << "\n";
    }
  }

  /// @brief Writes the documentation as a Unix manual page.
  /// @param output Stream receiving the generated document.
  void writeManDocumentation(std::ostream &output) const {
    const CommandParser &root = *parent_command_parser;
    output << ".TH \"" << root.name << "\" \"1\" \"" << currentDate() << "\" \""
           << root.name << ' ' << FINANCE_CORE_VERSION
           << "\" \"User Commands\"\n";
    output << ".SH NAME\n" << root.name << " \\- " << root.summary << "\n";
    output << ".SH SYNOPSIS\n";
    writePlainSynopsis(output, root);
    output << ".SH DESCRIPTION\n" << root.summary << "\n";
    output << ".SH COMMANDS\n";
    writeManCommands(output, root);
    output << ".SH GLOBAL OPTIONS\n";
    writeManOption(output, "-h, --help", "Display summary help and exit.");
    for (const ArgumentParser &argument : root.argument_parsers) {
      writeManOption(output, argumentLabel(argument), argument.summary);
    }
    output << ".SH EXAMPLES\n";
    writeManExamples(output, root);
  }

  /// @brief Writes a manual-page option entry.
  /// @param output Stream receiving the option entry.
  /// @param label Display label for the option.
  /// @param summary Description of the option.
  static void writeManOption(std::ostream &output, const std::string &label,
                             const std::string &summary) {
    output << ".TP\n\\fB" << label << "\\fR\n" << summary << '\n';
  }

  /// @brief Writes commands and their options as manual-page entries.
  /// @param output Stream receiving the command hierarchy.
  /// @param parser Root of the command subtree to render.
  static void writeManCommands(std::ostream &output,
                               const CommandParser &parser) {
    for (const auto &command : parser.subcommand_parsers) {
      writeManOption(output, command->name, command->summary);
      for (const ArgumentParser &argument : command->argument_parsers) {
        writeManOption(output, argumentLabel(argument), argument.summary);
      }
      writeManCommands(output, *command);
    }
  }

  /// @brief Writes examples from the root command tree as manual-page entries.
  /// @param output Stream receiving the examples.
  /// @param root Root command parser.
  static void writeManExamples(std::ostream &output,
                               const CommandParser &root) {
    std::vector<const CommandExample *> examples;
    collectExamples(root, examples);
    for (const CommandExample *example : examples) {
      output << ".TP\n"
             << example->description << "\n.nf\n"
             << example->command << "\n.fi\n";
    }
  }

  /// Records whether an option callback reported an invalid value.
  bool m_option_error = false;
};

} // namespace finance::command
