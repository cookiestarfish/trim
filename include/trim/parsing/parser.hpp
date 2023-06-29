#pragma once
#include <trim/container/labels.hpp>
#include <trim/container/tree.hpp>

#include <concepts>
#include <span>

namespace trim
{
  struct Parse_Error
  {
    std::string message;
    std::size_t position;
    std::size_t extent;

    Parse_Error() = default;

    constexpr Parse_Error(std::string message, std::size_t position, std::size_t extent) noexcept
      : message(std::move(message))
      , position(position)
      , extent(extent)
    {}
  };

  struct Parse_Result
  {
    Tree tree;
    size_type root;
    Labels node_labels;
    Labels edge_labels;
    std::vector<Parse_Error> errors;

    Parse_Result() = default;

    constexpr Parse_Result(Tree tree, size_type root, Labels node_labels, Labels edge_labels)
      : tree(std::move(tree))
      , root(root)
      , node_labels(std::move(node_labels))
      , edge_labels(std::move(edge_labels))
      , errors()
    {}

    constexpr Parse_Result(std::vector<Parse_Error> errors)
      : tree()
      , root(0)
      , node_labels()
      , edge_labels()
      , errors(std::move(errors))
    {}
  };

  template<typename T>
  concept Is_Parser = requires(T const& parser, std::string_view string) {
    // clang-format off
    { parser.parse(string) } -> std::same_as<Parse_Result>;
    // clang-format on
  };

} // namespace trim
