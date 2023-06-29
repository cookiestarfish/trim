#pragma once
#include <trim/color/rgb.hpp>
#include <trim/util/assert.hpp>

#include <cstdint>
#include <optional>
#include <string_view>

namespace trim
{
  [[nodiscard]] constexpr std::optional<Color_RGB> parse_color_hex(std::string_view hex)
  {
    auto const is_hex_digit = [](char character) noexcept -> bool {
      if(character >= 'a' && character <= 'f')
        return true;
      if(character >= 'A' && character <= 'F')
        return true;
      if(character >= '0' && character <= '9')
        return true;
      return false;
    };

    auto const digit_value = [](char character) noexcept -> int {
      if(character >= 'a' && character <= 'f')
        return character - 'a' + 10;
      if(character >= 'A' && character <= 'F')
        return character - 'A' + 10;
      if(character >= '0' && character <= '9')
        return character - '0';
      TRIM_ASSERT(false);
    };

    if(!hex.starts_with("#"))
      return std::nullopt;

    hex.remove_prefix(1);
    std::uint_least32_t value = 0;

    while(!hex.empty()) {
      int digit = hex.front();
      hex.remove_prefix(1);

      if(!is_hex_digit(digit))
        return std::nullopt;
      digit = digit_value(digit);
      value = value * 16 + digit;
    }

    return Color_RGB(static_cast<std::uint_least32_t>(value));
  }

  [[nodiscard]] constexpr std::optional<Color_RGB> parse_color_name(std::string_view name)
  {
    struct Color_Pair
    {
      std::string_view name;
      Color_RGB value;
    };

    // clang-format off
    constexpr Color_Pair colors[] = {
      Color_Pair {"none",               Color_RGB::NONE},
      Color_Pair {"red",                Color_RGB::RED},
      Color_Pair {"cherryred",          Color_RGB::CHERRY_RED},
      Color_Pair {"bubblegumpink",      Color_RGB::BUBBLEGUM_PINK},
      Color_Pair {"green",              Color_RGB::GREEN},
      Color_Pair {"blue",               Color_RGB::BLUE},
      Color_Pair {"black",              Color_RGB::BLACK},
      Color_Pair {"white",              Color_RGB::WHITE},
      Color_Pair {"gray",               Color_RGB::GRAY},
      Color_Pair {"pink",               Color_RGB::PINK},
      Color_Pair {"purple",             Color_RGB::PURPLE},
      Color_Pair {"cyan",               Color_RGB::CYAN},
      Color_Pair {"brown",              Color_RGB::BROWN},
      Color_Pair {"peach",              Color_RGB::PEACH},
      Color_Pair {"yellow",             Color_RGB::YELLOW},
      Color_Pair {"orange",             Color_RGB::ORANGE},
      Color_Pair {"burntorange",        Color_RGB::BURNT_ORANGE},
      Color_Pair {"rainbow",            Color_RGB::RAINBOW}
    };
    // clang-format on

    for(Color_Pair const& color : colors) {
      if(name == color.name) {
        return color.value;
      }
    }

    return std::nullopt;
  }
} // namespace trim
