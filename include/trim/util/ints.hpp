#pragma once
#include <cstdint>
#include <numeric>
#include <type_traits>

namespace trim
{
  using coord_type = long;
  using size_type = std::size_t;
  using ssize_type = std::make_signed_t<size_type>;

  static constexpr coord_type max_coord = std::numeric_limits<coord_type>::max();
  static constexpr coord_type min_coord = std::numeric_limits<coord_type>::min();
} // namespace trim
