#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace finance::presentation {

/**
 * @brief Alignment applied to a rendered cell within its column.
 */
enum Alignment {
  /**
   * @brief Place cell text before its padding.
   */
  LEFT,
  /**
   * @brief Center cell text, placing any odd extra padding on the right.
   */
  MIDDLE,
  /**
   * @brief Place cell text after its padding.
   */
  RIGHT
};

/**
 * @brief Output formats supported by the table renderer.
 */
enum Format {
  /**
   * @brief Render a plain-text table.
   */
  TEXT
};

/**
 * @brief Describes how one record value is rendered as a table column.
 *
 * Column width is based on the heading and rendered cell values.
 * @tparam RECORD Type of records processed by this column.
 */
template <typename RECORD> struct TableColumn {
  /**
   * @brief Heading printed for this column and used as its minimum width.
   */
  std::string mHeading;

  /**
   * @brief Alignment used for rendered cell values; defaults to left.
   */
  Alignment mAlignment = Alignment::LEFT;

  /**
   * @brief Converts a record into this column's plain-text cell value.
   */
  std::function<std::string(const RECORD &)> mProcessor;
};

/**
 * @brief Defines the ordered columns and divider used to render a table.
 *
 * The default divider is a tab character.
 * @tparam RECORD Type of records accepted by the table's columns.
 */
template <typename RECORD> struct TableDefinition {
  /**
   * @brief Text placed between adjacent columns; defaults to a tab.
   */
  std::string mDivider = "\t";

  /**
   * @brief Columns in the order their headings and values are rendered.
   */
  std::vector<TableColumn<RECORD>> mColumns;
};

/**
 * @brief Renders records using a table definition.
 *
 * Only plain-text output is supported. Select a format before rendering.
 * @tparam RECORD Type of records rendered by the table.
 */
template <typename RECORD> class TableRenderer {
public:
  /**
   * @brief Creates a renderer with a copy of the supplied table definition.
   * @param arDefinition Column and divider configuration for rendered tables.
   */
  TableRenderer(const TableDefinition<RECORD> &arDefinition)
      : mDefinition(arDefinition) {}

  /**
   * @brief Selects the output format used by subsequent render calls.
   * @param aFormat Format to use; currently only Format::TEXT is supported.
   */
  void format(const Format aFormat) { mFormat = aFormat; }

  /**
   * @brief Writes the table heading and records to the supplied stream.
   *
   * The heading is written even when no records are supplied. Record order is
   * preserved.
   * @param arOutput Stream receiving the rendered table.
   * @param arRecords Records to render.
   */
  void render(std::ostream &arOutput,
              const std::vector<RECORD> &arRecords) const {
    switch (mFormat) {
    case Format::TEXT:
      renderPlainText(arOutput, arRecords);
      break;

    default:
      break;
    }
  }

private:
  /**
   * @brief Immutable copy of the table's column and divider configuration.
   */
  const TableDefinition<RECORD> mDefinition;

  /**
   * @brief Format selected for rendering; set through format before render.
   */
  Format mFormat;

  /**
   * @brief Writes a plain-text table using the configured columns.
   * @param arOutput Stream receiving the rendered table.
   * @param arRecords Records to render.
   */
  void renderPlainText(std::ostream &arOutput,
                       const std::vector<RECORD> &arRecords) const {

    // calculate the column widths
    int i = 0;
    std::vector<std::size_t> columnWidths(mDefinition.mColumns.size());
    for (const auto &column : mDefinition.mColumns) {
      // min column width equal to heading length
      columnWidths[i] = column.mHeading.size();

      for (const auto &record : arRecords) {
        columnWidths[i] =
            std::max(columnWidths[i],
                     mDefinition.mColumns.at(i).mProcessor(record).size());
      }

      i++;
    }

    // render the headings
    i = 0;
    for (const auto &column : mDefinition.mColumns) {
      // add the divider for additional columns
      if (i != 0) {
        arOutput << mDefinition.mDivider;
      }

      // render the heading
      arOutput << column.mHeading
               << std::string(columnWidths[i] - column.mHeading.size(), ' ');
      i++;
    }

    // render the records
    for (const auto &record : arRecords) {
      i = 0;
      arOutput << std::endl;
      for (const auto &column : mDefinition.mColumns) {
        // add the divider for additional columns
        if (i != 0) {
          arOutput << mDefinition.mDivider;
        }

        std::string output = column.mProcessor(record);
        const std::size_t padding = columnWidths[i] - output.size();
        switch (column.mAlignment) {
        case Alignment::LEFT:
          arOutput << output << std::string(padding, ' ');
          break;
        case Alignment::MIDDLE: {
          const std::size_t leftPadding = padding / 2;
          const std::size_t rightPadding = padding - leftPadding;
          arOutput << std::string(leftPadding, ' ') << output
                   << std::string(rightPadding, ' ');
          break;
        }
        case Alignment::RIGHT:
          arOutput << std::string(padding, ' ') << output;
          break;
        }
        i++;
      }
    }
  }
};
} // namespace finance::presentation