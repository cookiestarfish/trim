#pragma once
#include <trim/sprite/sprite.hpp>
#include <trim/style/style.hpp>
#include <trim/util/geometry.hpp>
#include <trim/util/ints.hpp>
#include <trim/util/split.hpp>
#include <trim/util/string.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace trim
{
  struct Text_Sprite
  {
    private:

    std::vector<trim::String> m_lines {};
    coord_type m_height {};
    coord_type m_width {};

    public:

    Text_Sprite() = default;

    constexpr Text_Sprite(std::string_view text, coord_type height, coord_type width)
      : m_lines()
      , m_height(height)
      , m_width(width)
    {
      trim::split_string_by_newline(text, [&](std::string_view line) {
        m_lines.push_back(trim::String(line));
      });
    }

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return Rect(Point::origin, Point(m_height, m_width));
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(trim::envelopes(rect(), cursor));
      coord_type const num_lines = m_lines.size();

      // TODO: implement vertical alignment

      if(cursor.line >= num_lines)
        return {};

      std::string_view line = std::string_view(m_lines[cursor.line]);

      switch(style.text_align) {
        case Text_Alignment::NONE:
        case Text_Alignment::LEFT: {
          if(cursor.column < style.node_horizontal_padding)
            return {};
          coord_type index = cursor.column - style.node_horizontal_padding;
          std::string_view character = (index < coord_type(line.size()) ? std::string_view(line.data() + index, 1) : "");
          return Draw_Result {character, style.text_color};
        }

        case Text_Alignment::CENTER: {
          if(cursor.column < style.node_horizontal_padding)
            return {};
          coord_type max_width = (m_width + 1) - style.node_horizontal_padding * 2;
          coord_type margin = max_width - line.size();
          if(cursor.column - style.node_horizontal_padding < (margin / 2))
            return {};

          coord_type index = cursor.column - style.node_horizontal_padding - (margin / 2);
          std::string_view character = index < coord_type(line.size()) ? std::string_view(line.data() + index, 1) : "";
          return Draw_Result {character, style.text_color};
        }

        case Text_Alignment::RIGHT: {
          if(cursor.column < style.node_horizontal_padding)
            return {};
          coord_type max_width = (m_width + 1) - style.node_horizontal_padding * 2;
          coord_type margin = max_width - line.size();
          if(cursor.column - style.node_horizontal_padding < margin)
            return {};

          coord_type index = cursor.column - style.node_horizontal_padding - margin;
          std::string_view character = index < coord_type(line.size()) ? std::string_view(line.data() + index, 1) : "";
          return Draw_Result {character, style.text_color};
        }
      }

      TRIM_ASSERT(false);
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return Sprite_Category::TEXT;
    }
  };

  static_assert(Is_Sprite<Text_Sprite>);
} // namespace trim
