#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace trim
{
  struct Tree
  {
    using size_type = std::size_t;
    using ssize_type = std::make_signed_t<size_type>;
    using list_type = std::vector<size_type>;

    std::vector<list_type> adjacency {};

    Tree() = default;

    constexpr Tree(size_type n)
      : adjacency(n, list_type())
    {}

    constexpr void clear()
    {
      adjacency.clear();
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
      return adjacency.size();
    }

    [[nodiscard]] constexpr ssize_type ssize() const noexcept
    {
      return static_cast<ssize_type>(adjacency.size());
    }

    [[nodiscard]] constexpr size_type get_child(size_type node, size_type index) const
    {
      return adjacency[node][index];
    }

    [[nodiscard]] constexpr size_type num_children(size_type node) const
    {
      return adjacency[node].size();
    }

    constexpr void add_child(size_type parent, size_type child)
    {
      adjacency[parent].push_back(child);
    }

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Tree const& tree)
    {
      stream << "tree[size = " << tree.size() << "] {\n";
      for(size_type i = 0; i < tree.size(); ++i) {
        for(size_type j : tree.adjacency[i]) {
          stream << i << " -> " << j << '\n';
        }
      }
      stream << "}\n";
      return stream;
    }
  };

  template<std::invocable<std::size_t> Callback>
  constexpr void tree_visit_postorder(Tree const& tree, std::size_t root, Callback callback)
  {
    using bit_type = char8_t;

    std::size_t const N = tree.size();
    auto visited = std::vector<bit_type>(N, false);

    auto const recurse = [&](auto& rec, std::size_t curr) -> void {
      visited[curr] = true;
      for(std::size_t i = 0; i < tree.num_children(curr); ++i) {
        std::size_t child = tree.get_child(curr, i);
        if(!visited[child]) {
          rec(rec, child);
        }
      }
      callback(curr);
    };
    recurse(recurse, root);
  }

  template<std::invocable<std::size_t> Callback>
  constexpr void tree_visit_preorder(Tree const& tree, std::size_t root, Callback callback)
  {
    using bit_type = char8_t;

    std::size_t const N = tree.size();
    auto visited = std::vector<bit_type>(N, false);

    auto const recurse = [&](auto& rec, std::size_t curr) -> void {
      visited[curr] = true;
      callback(curr);
      for(std::size_t i = 0; i < tree.num_children(curr); ++i) {
        std::size_t child = tree.get_child(curr, i);
        if(!visited[child]) {
          rec(rec, child);
        }
      }
    };
    recurse(recurse, root);
  }

  template<std::ranges::range Output_Range>
  constexpr void tree_compute_levels(Tree const& tree, std::size_t root, Output_Range& range)
  {
    range[root] = 0;
    trim::tree_visit_preorder(tree, root, [&](std::size_t curr) -> void {
      for(std::size_t i = 0; i < tree.num_children(curr); ++i) {
        std::size_t child = tree.get_child(curr, i);
        range[child] = range[curr] + 1;
      }
    });
  }
} // namespace trim
