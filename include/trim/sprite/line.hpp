#pragma once
#include <trim/sprite/sprite.hpp>
#include <trim/style/style.hpp>
#include <trim/util/geometry.hpp>

namespace trim
{
  struct Horizontal_Line_Sprite
  {
    private:

    coord_type m_width {};
    bool m_ignore_start {};
    bool m_ignore_end {};

    public:

    constexpr Horizontal_Line_Sprite(coord_type width, bool ignore_start, bool ignore_end) noexcept
      : m_width(width)
      , m_ignore_start(ignore_start)
      , m_ignore_end(ignore_end)
    {}

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return Rect(Point::origin, Point(0, m_width));
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(trim::envelopes(rect(), cursor));

      if(cursor == Point::origin && m_ignore_start)
        return {};

      if(cursor == Point(0, m_width) && m_ignore_end)
        return {};

      return Draw_Result {style.horizontal_line, style.branch_color};
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return Sprite_Category::BRANCH;
    }
  };

  struct Vertical_Line_Sprite
  {
    private:

    coord_type m_height {};
    bool m_ignore_start {};
    bool m_ignore_end {};

    public:

    constexpr Vertical_Line_Sprite(coord_type height, bool ignore_start, bool ignore_end) noexcept
      : m_height(height)
      , m_ignore_start(ignore_start)
      , m_ignore_end(ignore_end)
    {}

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return Rect(Point::origin, Point(m_height, 0));
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(envelopes(rect(), cursor));

      if(cursor == Point::origin && m_ignore_start)
        return {};

      if(cursor == Point(m_height, 0) && m_ignore_end)
        return {};

      return Draw_Result {style.vertical_line, style.branch_color};
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return Sprite_Category::BRANCH;
    }
  };

  static_assert(Is_Sprite<Horizontal_Line_Sprite>);
  static_assert(Is_Sprite<Vertical_Line_Sprite>);
} // namespace trim
