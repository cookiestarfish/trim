#pragma once
#include <trim/sprite/box.hpp>
#include <trim/sprite/composite.hpp>
#include <trim/sprite/sprite.hpp>
#include <trim/sprite/text.hpp>
#include <trim/sprite/transform.hpp>

#include <string_view>
#include <vector>

namespace trim
{
  struct Node_Sprite
  {
    private:

    Composite_Sprite m_composite {};

    public:

    Node_Sprite() = default;

    constexpr Node_Sprite(coord_type height, coord_type width, bool is_top_connected, bool is_bot_connected, std::string_view text)
      : m_composite()
    {
      std::vector<Sprite> sprites {};

      Sprite box_sprite = Box_Sprite(height, width, is_top_connected, is_bot_connected);
      Sprite text_sprite = Text_Sprite(text, height - 2, width - 2);
      sprites.push_back(Translate_Sprite(std::move(text_sprite), 1, 1));
      sprites.push_back(std::move(box_sprite));
      m_composite = Composite_Sprite(std::move(sprites));
    }

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return m_composite.rect();
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(trim::envelopes(rect(), cursor));
      return m_composite.draw(style, cursor);
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return Sprite_Category::NODE | Sprite_Category::TEXT;
    }
  };

  static_assert(Is_Sprite<Node_Sprite>);
} // namespace trim
