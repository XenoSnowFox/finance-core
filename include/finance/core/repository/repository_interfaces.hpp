#pragma once

#include <concepts>
#include <optional>
#include <vector>

#include "finance/core/interfaces.hpp"

namespace finance::core::repository {

/**
 * @brief Interface for retrieving records from a repository.
 *
 * @tparam IDENTIFIER Type used to identify records.
 * @tparam RECORD Record type, which must be identifiable by IDENTIFIER.
 */
template <typename IDENTIFIER, typename RECORD>
  requires std::derived_from<RECORD, Identifiable<IDENTIFIER>>
class IFindableRepository {
public:
  virtual ~IFindableRepository() = default;
  /**
   * @brief Finds a record by its identifier.
   *
   * @param identifier Identifier of the record to find.
   * @return The matching record, or an empty optional if no record is found.
   */
  std::optional<RECORD>
  find_by_identifier(const IDENTIFIER &identifier) const override;

  /**
   * @brief Finds all records.
   *
   * @return All records in the repository.
   */
  std::vector<RECORD> find_all() const override;
};
} // namespace finance::core::repository
