#pragma once
#include <string_view>

namespace trim
{
  template<typename Fn>
  constexpr void split_string_by_newline(std::string_view string, Fn callback)
  {
    std::size_t begin = 0;
    std::size_t length = 0;
    std::string_view remaining = string;
    while(!remaining.empty()) {
      if(remaining.starts_with("\n")) {
        callback(string.substr(begin, length));
        begin += length + 1;
        length = 0;
        remaining.remove_prefix(1);
      } else {
        length += 1;
        remaining.remove_prefix(1);
      }
    }

    if(length > 0)
      callback(string.substr(begin, length));
  }

  [[nodiscard]] constexpr std::string_view prefix_before_newline(std::string_view string) noexcept
  {
    std::size_t length = 0;
    std::string_view remaining = string;
    while(!remaining.empty()) {
      if(remaining.starts_with("\n")) {
        return string.substr(0, length);
      }

      remaining.remove_prefix(1);
      length += 1;
    }

    return string;
  }
} // namespace trim
