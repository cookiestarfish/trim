#pragma once
#include <trim/util/string.hpp>

#include <algorithm>
#include <concepts>
#include <numeric>

namespace trim
{
  template<std::unsigned_integral T>
  constexpr char* format_integer(char* out, T value) noexcept
  {
    static_assert(std::numeric_limits<T>::digits10 <= 20);

    if(value == 0) {
      *out = '0';
      return out + 1;
    }

    char buffer[48] {};
    int n = 0;
    while(value > 0) {
      buffer[n] = (value % 10) + '0';
      value /= 10;
      n += 1;
    }

    std::reverse(buffer, buffer + n);
    std::copy(buffer, buffer + n, out);
    return out + n;
  }

  template<std::unsigned_integral T>
  [[nodiscard]] constexpr trim::String format_integer_to_string(T value)
  {
    char buffer[48] {};
    char* out = trim::format_integer(buffer, value);
    return trim::String(buffer, out - buffer);
  }

} // namespace trim
