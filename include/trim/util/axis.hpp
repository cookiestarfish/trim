#pragma once
#include <trim/util/assert.hpp>
#include <trim/util/ints.hpp>

#include <string_view>

namespace trim
{
  enum class Axis
  {
    HORIZONTAL,
    VERTICAL
  };

  [[nodiscard]] constexpr std::string_view to_string(Axis axis) noexcept
  {
    switch(axis) {
      case Axis::HORIZONTAL: return "HORIZONTAL";
      case Axis::VERTICAL: return "VERTICAL";
    }
    TRIM_ASSERT(false);
  }

  template<typename Stream>
  constexpr Stream& operator<<(Stream& stream, Axis axis)
  {
    return stream << trim::to_string(axis);
  }

  [[nodiscard]] constexpr bool axis_aligned(coord_type l1, coord_type c1, coord_type l2, coord_type c2) noexcept
  {
    return (l1 == l2) || (c1 == c2);
  }

  [[nodiscard]] constexpr Axis axis(coord_type l1, coord_type c1, coord_type l2, coord_type c2) noexcept
  {
    if(l1 == l2)
      return Axis::HORIZONTAL;
    if(c1 == c2)
      return Axis::VERTICAL;

    TRIM_ASSERT(false);
  }

} // namespace trim
