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
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  virtual ~IFindableRepository() = default;
  /**
   * @brief Finds a record by its identifier.
   *
   * @param identifier Identifier of the record to find.
   * @return The matching record, or an empty optional if no record is found.
   */
  virtual std::optional<RECORD>
  find_by_identifier(const IDENTIFIER &identifier) const = 0;

  /**
   * @brief Finds all records.
   *
   * @return All records in the repository.
   */
  virtual std::vector<RECORD> find_all() const = 0;
};

/**
 * @brief Interface for removing records from a repository.
 *
 * @tparam IDENTIFIER Type used to identify records.
 */
template <typename IDENTIFIER> class IDeletableRepository {
public:
  /**
   * @brief Destroys the repository through a base-class pointer.
   */
  virtual ~IDeletableRepository() = default;

  /**
   * @brief Deletes a record by its identifier.
   *
   * @param identifier Identifier of the record to delete.
   */
  virtual void delete_by_identifier(const IDENTIFIER &identifier) = 0;

  /**
   * @brief Deletes all records from the repository.
   */
  virtual void delete_all() = 0;
};
} // namespace finance::core::repository
