#pragma once
#include <trim/sprite/sprite.hpp>

namespace trim
{
  struct Translate_Sprite
  {
    private:

    Sprite m_sprite {};
    coord_type m_lines {};
    coord_type m_columns {};

    public:

    Translate_Sprite() = default;

    constexpr Translate_Sprite(Sprite sprite, coord_type lines, coord_type columns) noexcept
      : m_sprite(std::move(sprite))
      , m_lines(lines)
      , m_columns(columns)
    {}

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return trim::translate(m_sprite.rect(), m_lines, m_columns);
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(trim::envelopes(rect(), cursor));
      return m_sprite.draw(style, trim::translate(cursor, -m_lines, -m_columns));
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return m_sprite.category();
    }
  };

  static_assert(Is_Sprite<Translate_Sprite>);
} // namespace trim
