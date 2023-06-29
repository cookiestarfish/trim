#pragma once
#include <trim/sprite/sprite.hpp>
#include <trim/style/style.hpp>
#include <trim/util/geometry.hpp>
#include <trim/util/ints.hpp>
#include <trim/util/string.hpp>

#include <string_view>

namespace trim
{
  struct Character_Sprite
  {
    private:

    trim::String m_character {};

    public:

    Character_Sprite() = default;

    constexpr Character_Sprite(trim::String character) noexcept
      : m_character(std::move(character))
    {}

    constexpr Character_Sprite(std::string_view character)
      : m_character(trim::String(character))
    {}

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return Rect(Point::origin, Point::origin);
    }

    [[nodiscard]] constexpr Draw_Result draw([[maybe_unused]] Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(trim::envelopes(rect(), cursor));
      return Draw_Result {std::string_view(m_character), style.text_color};
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return Sprite_Category::TEXT;
    }
  };

  static_assert(Is_Sprite<Character_Sprite>);
} // namespace trim
