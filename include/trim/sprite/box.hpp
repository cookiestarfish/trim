#pragma once
#include <trim/sprite/sprite.hpp>
#include <trim/style/style.hpp>
#include <trim/util/geometry.hpp>
#include <trim/util/ints.hpp>
#include <type_traits>

namespace trim
{
  struct Box_Sprite
  {
    private:

    coord_type height {};
    coord_type width {};
    bool is_top_connected {};
    bool is_bot_connected {};

    public:

    Box_Sprite() = default;

    constexpr Box_Sprite(coord_type height, coord_type width, bool is_top_connected, bool is_bot_connected) noexcept
      : height(height)
      , width(width)
      , is_top_connected(is_top_connected)
      , is_bot_connected(is_bot_connected)
    {}

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return Rect(Point::origin, Point(height, width));
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(trim::envelopes(rect(), cursor));

      bool is_left = (cursor.column == 0);
      bool is_right = (cursor.column == width);
      bool is_top = (cursor.line == 0);
      bool is_bot = (cursor.line == height);
      bool is_top_left = is_left && is_top;
      bool is_top_right = is_right && is_top;
      bool is_bot_left = is_left && is_bot;
      bool is_bot_right = is_right && is_bot;
      bool is_any = (is_left || is_right || is_top || is_bot);

      if(!is_any)
        return {};

      if(is_top_left)
        return Draw_Result {style.box_top_left_corner, style.box_color};

      if(is_top_right)
        return Draw_Result {style.box_top_right_corner, style.box_color};

      if(is_bot_left)
        return Draw_Result {style.box_bot_left_corner, style.box_color};

      if(is_bot_right)
        return Draw_Result {style.box_bot_right_corner, style.box_color};

      Point const top_connection = Point(0, width / 2);
      Point const bot_connection = Point(height, width / 2);

      if(is_top_connected && cursor == top_connection)
        return Draw_Result {style.top_connection, style.box_color};

      if(is_bot_connected && cursor == bot_connection)
        return Draw_Result {style.bot_connection, style.box_color};

      if(is_top || is_bot)
        return Draw_Result {style.box_horizontal_line, style.box_color};

      if(is_left || is_right)
        return Draw_Result {style.box_vertical_line, style.box_color};

      TRIM_ASSERT(false);
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return Sprite_Category::NODE;
    }
  };

  static_assert(Is_Sprite<Box_Sprite>);
} // namespace trim
