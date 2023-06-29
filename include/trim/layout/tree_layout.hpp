#pragma once
#include <trim/util/geometry.hpp>
#include <trim/util/ints.hpp>

#include <vector>

namespace trim
{
  struct Node_Layout
  {
    Rect rect {};

    Node_Layout() = default;

    constexpr Node_Layout(Rect rect) noexcept
      : rect(rect)
    {}

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Node_Layout const& node)
    {
      return stream << "node(" << node.rect << ")";
    }
  };

  struct Tree_Layout
  {
    using size_type = std::size_t;
    using ssize_type = std::make_signed_t<std::size_t>;
    using value_type = Node_Layout;
    using reference = value_type&;
    using const_reference = value_type const&;
    using pointer = value_type*;
    using const_pointer = value_type const*;

    std::vector<value_type> layout {};

    Tree_Layout() = default;

    explicit constexpr Tree_Layout(size_type n)
      : layout(n)
    {}

    explicit constexpr Tree_Layout(std::vector<value_type> layout) noexcept
      : layout(std::move(layout))
    {}

    [[nodiscard]] constexpr size_type size() const noexcept
    {
      return layout.size();
    }

    [[nodiscard]] constexpr ssize_type ssize() const noexcept
    {
      return static_cast<ssize_type>(layout.size());
    }

    [[nodiscard]] constexpr reference operator[](size_type index) & noexcept
    {
      TRIM_ASSERT(index < size());
      return layout[index];
    }

    [[nodiscard]] constexpr const_reference operator[](size_type index) const& noexcept
    {
      TRIM_ASSERT(index < size());
      return layout[index];
    }

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Tree_Layout const& tree)
    {
      stream << "tree_layout[size = " << tree.size() << "] {\n";
      for(size_type i = 0; i < tree.size(); ++i) {
        stream << i << ": " << tree[i] << '\n';
      }
      stream << "}\n";
      return stream;
    }
  };
} // namespace trim
