# finance-core(1) — User Commands

## NAME

**finance-core** — A self-hosted personal and solo business finance system.

## SYNOPSIS

```text
finance-core [GLOBAL_OPTIONS] COMMAND [ARGS...]
finance-core doc [OPTIONS]
finance-core currency
```

## DESCRIPTION

A self-hosted personal and solo business finance system.

## COMMANDS

- **doc** — Generate documentation for the tool.
  - `--format <format>` — Select the output format: html, md, man, or txt.
  - `-o, --output <filename>` — Write documentation to this file. Defaults to ./docs/finance-core.txt for text, ./docs/finance-core.html for HTML, ./docs/finance-core.md for Markdown, and ./docs/finance-core.1 for man pages.
- **currency** — Command for managing currency information.

## GLOBAL OPTIONS

- `-h`, `--help` — Display summary help and exit.
- `-v, --verbose` — Enable verbose logging.

## EXAMPLES

Display the available commands and options.

```sh
finance-core --help
```

Generate a single-page HTML manual.

```sh
finance-core doc --format html --output docs/finance-core.html
```


---

v1.0.0  
October 2026  
finance-core(1)
