#pragma once

namespace finance::core {

/**
 * @brief Base type for objects identified by a value.
 *
 * @tparam IDENTIFIER Type used to represent the object's identifier.
 */
template <typename IDENTIFIER> struct Identifiable {
  /**
   * @brief Destroys the object through a base-class pointer.
   */
  virtual ~Identifiable() = default;

  /**
   * @brief Identifier associated with the object.
   */
  IDENTIFIER identifier;
};
} // namespace finance::core