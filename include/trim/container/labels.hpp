#pragma once
#include <trim/util/ints.hpp>
#include <trim/util/string.hpp>

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

namespace trim
{
  /*!
   * Maps a node or edge id to its label.
   * Node identifiers are [0, N) where N is the number of nodes.
   * Edge identifiers are [0, N) where N is the number of nodes.
   * The edge with identifier 'x' is the parent edge of the x-th node.
   * The root node has no parent edge, accessing its label returns
   * the empty string.
   */
  struct Labels
  {
    using reference = trim::String&;
    using const_reference = trim::String const&;

    std::vector<trim::String> labels;

    Labels() = default;

    constexpr Labels(size_type n)
      : labels(n)
    {}

    [[nodiscard]] constexpr reference operator[](size_type id)
    {
      return labels[id];
    }

    [[nodiscard]] constexpr const_reference operator[](size_type id) const
    {
      return labels[id];
    }

    [[nodiscard]] constexpr std::string_view operator()(size_type id) const
    {
      return std::string_view(labels[id]);
    }
  };
} // namespace trim
