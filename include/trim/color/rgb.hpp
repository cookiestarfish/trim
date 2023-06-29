#pragma once
#include <trim/util/splitmix64.hpp>

#include <compare>
#include <cstdint>

namespace trim
{
  struct Color_RGB
  {
    static constinit const Color_RGB NONE;

    static constinit const Color_RGB PINK;
    static constinit const Color_RGB RED;
    static constinit const Color_RGB CHERRY_RED;
    static constinit const Color_RGB BUBBLEGUM_PINK;
    static constinit const Color_RGB ORANGE;
    static constinit const Color_RGB BURNT_ORANGE;
    static constinit const Color_RGB YELLOW;
    static constinit const Color_RGB BROWN;
    static constinit const Color_RGB PEACH;
    static constinit const Color_RGB GREEN;
    static constinit const Color_RGB CYAN;
    static constinit const Color_RGB BLUE;
    static constinit const Color_RGB PURPLE;
    static constinit const Color_RGB WHITE;
    static constinit const Color_RGB GRAY;
    static constinit const Color_RGB BLACK;
    static constinit const Color_RGB RAINBOW;

    int red;
    int green;
    int blue;

    Color_RGB() = default;

    explicit constexpr Color_RGB(int red, int green, int blue) noexcept
      : red(red)
      , green(green)
      , blue(blue)
    {}

    explicit constexpr Color_RGB(std::uint_least32_t compressed_color) noexcept
      : red((compressed_color >> 16) & 0xFF)
      , green((compressed_color >> 8) & 0xFF)
      , blue((compressed_color >> 0) & 0xFF)
    {}

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Color_RGB color)
    {
      return stream << "color(" << color.red << ", " << color.green << ", " << color.blue << ")";
    }

    bool operator==(Color_RGB const&) const = default;
    auto operator<=>(Color_RGB const&) const = default;
  };

  constexpr Color_RGB const Color_RGB::NONE = Color_RGB(0, 0, 0);
  constexpr Color_RGB const Color_RGB::PINK = Color_RGB(255, 192, 203);
  constexpr Color_RGB const Color_RGB::BUBBLEGUM_PINK = Color_RGB(0xF7, 0xC5, 0xCC);
  constexpr Color_RGB const Color_RGB::CHERRY_RED = Color_RGB(0xCC, 0x31, 0x3D);
  constexpr Color_RGB const Color_RGB::RED = Color_RGB(255, 0, 0);
  constexpr Color_RGB const Color_RGB::ORANGE = Color_RGB(255, 165, 0);
  constexpr Color_RGB const Color_RGB::BURNT_ORANGE = Color_RGB(0xEE, 0x4E, 0x34);
  constexpr Color_RGB const Color_RGB::YELLOW = Color_RGB(255, 255, 0);
  constexpr Color_RGB const Color_RGB::BROWN = Color_RGB(165, 42, 42);
  constexpr Color_RGB const Color_RGB::PEACH = Color_RGB(0xFC, 0xED, 0xDA);
  constexpr Color_RGB const Color_RGB::GREEN = Color_RGB(0, 255, 0);
  constexpr Color_RGB const Color_RGB::CYAN = Color_RGB(0, 255, 255);
  constexpr Color_RGB const Color_RGB::BLUE = Color_RGB(0, 0, 255);
  constexpr Color_RGB const Color_RGB::PURPLE = Color_RGB(128, 0, 128);
  constexpr Color_RGB const Color_RGB::WHITE = Color_RGB(255, 255, 255);
  constexpr Color_RGB const Color_RGB::BLACK = Color_RGB(0, 0, 1);
  constexpr Color_RGB const Color_RGB::GRAY = Color_RGB(127, 127, 127);

  // magic color used to trigger the rainbow effect
  constexpr Color_RGB const Color_RGB::RAINBOW = Color_RGB(215, 45, 109);

  [[nodiscard]] constexpr Color_RGB pick_rainbow(double ratio) noexcept
  {
    if(ratio > 1.0)
      ratio = 1.0;
    if(ratio < 0.0)
      ratio = 0.0;

    // we want to normalize ratio so that it fits in to 6 regions
    // where each region is 256 units long
    int normalized = int(ratio * 256 * 6);

    // find the region for this position
    int region = normalized / 256;

    // find the distance to the start of the closest region
    int x = normalized % 256;

    uint8_t r = 0, g = 0, b = 0;
    switch(region) {
      case 0:
        r = 255;
        g = 0;
        b = 0;
        g += x;
        break;
      case 1:
        r = 255;
        g = 255;
        b = 0;
        r -= x;
        break;
      case 2:
        r = 0;
        g = 255;
        b = 0;
        b += x;
        break;
      case 3:
        r = 0;
        g = 255;
        b = 255;
        g -= x;
        break;
      case 4:
        r = 0;
        g = 0;
        b = 255;
        r += x;
        break;
      case 5:
        r = 255;
        g = 0;
        b = 255;
        b -= x;
        break;
    }

    return Color_RGB(r, g, b);
  }

  [[nodiscard]] constexpr Color_RGB pick_rainbow(std::uint_least64_t seed) noexcept
  {
    std::uint_least64_t hash = trim::splitmix64(seed);
    return pick_rainbow((hash % 1024) / 1024.0);
  }

} // namespace trim
