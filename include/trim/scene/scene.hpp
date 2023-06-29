#pragma once
#include <trim/sprite/composite.hpp>
#include <trim/sprite/tree.hpp>
#include <trim/util/format_int.hpp>
#include <trim/util/geometry.hpp>

namespace trim
{
  struct Scene
  {
    private:

    Composite_Sprite m_composite {};

    public:

    Scene() = default;

    explicit constexpr Scene(std::vector<Sprite> sprites) //
      : m_composite(std::move(sprites))
    {}

    explicit constexpr Scene(Sprite sprite) //
      : m_composite(std::move(sprite))
    {}

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return m_composite.rect();
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor)
    {
      TRIM_ASSERT(envelopes(rect(), cursor));
      return m_composite.draw(style, cursor);
    }

    template<typename Stream>
    constexpr void draw(Stream& stream, Style const& style)
    {
      Rect const rect = this->rect();
      coord_type const line1 = top_line(rect);
      coord_type const line2 = bot_line(rect);
      coord_type const column1 = left_column(rect);
      coord_type const column2 = right_column(rect);
      std::array<char, 128> buffer {};

      auto const print_colored = [&buffer](std::string_view character, Color_RGB color) -> std::string_view {
        char* out = std::ranges::copy(std::string_view("\033[38;2;"), buffer.data()).out;
        out = format_integer(out, static_cast<unsigned>(color.red));
        *out++ = ';';
        out = format_integer(out, static_cast<unsigned>(color.green));
        *out++ = ';';
        out = format_integer(out, static_cast<unsigned>(color.blue));
        *out++ = 'm';
        out = std::ranges::copy(character, out).out;
        out = std::ranges::copy(std::string_view("\033[0;00m"), out).out;
        *out++ = '\0';
        return std::string_view(buffer.data(), out - buffer.data());
      };

      for(coord_type line = line1; line <= line2; ++line) {
        for(coord_type column = column1; column <= column2; ++column) {
          Draw_Result drawable = m_composite.draw(style, Point(line, column));

          if(drawable.character == "")
            drawable.character = " ";

          if(drawable.color != Color_RGB::NONE && drawable.character != " ") {
            stream << print_colored(drawable.character, drawable.color);
          } else {
            stream << drawable.character;
          }
        }

        stream << '\n';
      }
    }
  };
} // namespace trim
