#pragma once
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

namespace trim
{
  /*!
   * Stores a left or right contour of a subtree.
   * A contour is a list of vertices and offsets
   * The offset represents a positive/negative horizontal
   * displacement from the previous node in the contour.
   * The offset of the first node is always 0.
   *
   * Example:
   * Given the following tree
   * The left contour nodes are [0, 1, 2, 6]
   * The right contour nodes are [0, 6, 8, 7]
   *
   *               ┌───┐
   *               | 0 |
   *               └─┬─┘
   *    ┌──────┬─────┴┬──────────┐
   *  ┌─┴─┐  ┌─┴─┐  ┌─┴─┐      ┌─┴─┐
   *  | 1 |  | 3 |  | 4 |      | 8 |
   *  └─┬─┘  └───┘  └─┬─┘      └─┬─┘
   *    |             |      ┌───┴───┐
   *  ┌─┴─┐         ┌─┴─┐  ┌─┴─┐  ┌──┴──┐
   *  | 2 |         | 5 |  | 9 |  | 10  |
   *  └───┘         └─┬─┘  └───┘  └─────┘
   *               ┌──┴───┐
   *             ┌─┴─┐  ┌─┴─┐
   *             | 6 |  | 7 |
   *             └───┘  └───┘
   */
  struct Contour
  {
    public:

    struct Element
    {
      std::size_t node {};
      long offset {};

      Element() = default;

      constexpr Element(std::size_t node, long offset) noexcept
        : node(node)
        , offset(offset)
      {}

      bool operator==(Element const&) const = default;
      auto operator<=>(Element const&) const = default;
    };

    using size_type = std::size_t;
    using ssize_type = std::make_signed_t<size_type>;
    using list_type = std::vector<Element>;
    using value_type = Element;
    using reference = Element&;
    using const_reference = Element const&;
    using offset_type = long;
    using iterator = typename list_type::iterator;
    using const_iterator = typename list_type::const_iterator;

    private:

    list_type elements {};

    public:

    Contour() = default;

    constexpr Contour(list_type elements) noexcept
      : elements(std::move(elements))
    {}

    [[nodiscard]] constexpr size_type size() const
    {
      return elements.size();
    }

    [[nodiscard]] constexpr ssize_type ssize() const noexcept
    {
      return static_cast<ssize_type>(elements.size());
    }

    [[nodiscard]] constexpr reference operator[](size_type index)
    {
      return elements[index];
    }

    [[nodiscard]] constexpr const_reference operator[](size_type index) const
    {
      return elements[index];
    }

    constexpr void push_back(size_type node, offset_type offset)
    {
      elements.push_back(Element(node, offset));
    }

    constexpr void push_front(size_type node, offset_type offset)
    {
      elements.insert(elements.begin(), Element(node, offset));
    }

    constexpr void merge(Contour&& other)
    {
      offset_type offset1 = 0;
      offset_type offset2 = 0;

      size_type pos1 = 0;
      size_type pos2 = 0;

      while(pos1 < size() && pos2 < other.size()) {
        offset1 += elements[pos1].offset;
        offset2 += other[pos2].offset;
        ++pos1;
        ++pos2;
      }

      if(pos1 < other.size()) {
        push_back(other[pos1].node, (offset2 + other[pos2].offset) - offset1);
        ++pos1;
      }

      while(pos1 < other.size()) {
        push_back(other[pos1].node, other[pos1].offset);
        ++pos1;
      }
    }

    template<typename Width_Fn>
    [[nodiscard]] friend constexpr offset_type minimum_offset(Contour const& c1, Contour const& c2, Width_Fn width_map) noexcept
    {
      offset_type x1 = 0;
      offset_type x2 = 0;
      offset_type result = 0;

      size_type pos1 = 0;
      size_type pos2 = 0;

      while(pos1 < c1.size() && pos2 < c2.size()) {
        x1 += c1[pos1].offset;
        x2 += c2[pos2].offset;
        offset_type right_edge = x1 + width_map(c1[pos1].node);

        if(right_edge > x2)
          result = std::max(result, right_edge - x2);

        ++pos1;
        ++pos2;
      }

      return result;
    }

    [[nodiscard]] constexpr const_iterator begin() const
    {
      return elements.begin();
    }

    [[nodiscard]] constexpr const_iterator end() const
    {
      return elements.end();
    }

    constexpr void clear()
    {
      elements.clear();
      elements = list_type();
    }

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Contour const& contour)
    {
      offset_type offset = 0;
      char const* delim = "";

      stream << "contour[";
      for(Element const& element : contour) {
        stream << delim << "(" << element.node << ", " << offset << ")";
        delim = ", ";
        offset += element.offset;
      }
      stream << "]";
      return stream;
    }
  };
} // namespace trim
