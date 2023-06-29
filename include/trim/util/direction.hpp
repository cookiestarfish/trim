#pragma once
#include <trim/util/assert.hpp>
#include <trim/util/axis.hpp>
#include <trim/util/ints.hpp>

#include <cmath>
#include <string_view>

namespace trim
{
  enum class Direction
  {
    RIGHT,
    DOWN,
    LEFT,
    UP
  };

  [[nodiscard]] constexpr std::string_view to_string(Direction direction) noexcept
  {
    switch(direction) {
      case Direction::RIGHT: return "RIGHT";
      case Direction::DOWN: return "DOWN";
      case Direction::LEFT: return "LEFT";
      case Direction::UP: return "UP";
    }

    TRIM_ASSERT(false);
  }

  template<typename Stream>
  constexpr Stream& operator<<(Stream& stream, Direction direction)
  {
    return stream << trim::to_string(direction);
  }

  [[nodiscard]] constexpr Direction direction(coord_type l1, coord_type c1, coord_type l2, coord_type c2) noexcept
  {
    if(c2 == c1) {
      if(l2 > l1)
        return Direction::DOWN;
      return Direction::UP;
    } else if(l2 == l1) {
      if(c2 > c1)
        return Direction::RIGHT;
      return Direction::LEFT;
    } else {
      TRIM_ASSERT(false);
    }
  }

  [[nodiscard]] constexpr Direction reverse(Direction dir) noexcept
  {
    switch(dir) {
      case Direction::RIGHT: return Direction::LEFT;
      case Direction::DOWN: return Direction::UP;
      case Direction::LEFT: return Direction::RIGHT;
      case Direction::UP: return Direction::DOWN;
    }

    TRIM_ASSERT(false);
  }

  [[nodiscard]] constexpr coord_type line_magnitude(Direction dir) noexcept
  {
    switch(dir) {
      case Direction::RIGHT: return 0;
      case Direction::DOWN: return 1;
      case Direction::LEFT: return 0;
      case Direction::UP: return -1;
    }

    TRIM_ASSERT(false);
  }

  [[nodiscard]] constexpr coord_type column_magnitude(Direction dir) noexcept
  {
    switch(dir) {
      case Direction::RIGHT: return +1;
      case Direction::DOWN: return 0;
      case Direction::LEFT: return -1;
      case Direction::UP: return 0;
    }

    TRIM_ASSERT(false);
  }
} // namespace trim
