#pragma once
#include <trim/sprite/sprite.hpp>

#include <vector>

namespace trim
{
  struct Composite_Sprite
  {
    private:

    std::vector<Sprite> m_sprites {};
    Rect m_rect {};
    Sprite_Category m_category {};

    public:

    Composite_Sprite() = default;

    template<typename T>
      requires(!std::is_same_v<T, Composite_Sprite> && Is_Sprite<T>)
    explicit constexpr Composite_Sprite(T sprite)
      : m_sprites()
      , m_rect()
      , m_category()
    {
      m_sprites.push_back(Sprite(std::move(sprite)));
      m_rect = m_sprites[0].rect();
      m_category = m_sprites[0].category();
    }

    explicit constexpr Composite_Sprite(std::vector<Sprite> sprites) noexcept
      : m_sprites(std::move(sprites))
      , m_rect()
      , m_category()
    {
      coord_type min_line = max_coord;
      coord_type max_line = min_coord;
      coord_type min_col = max_coord;
      coord_type max_col = min_coord;

      for(Sprite const& s : m_sprites) {
        Rect curr = s.rect();
        min_line = std::min(min_line, top_line(curr));
        max_line = std::max(max_line, bot_line(curr));
        min_col = std::min(min_col, left_column(curr));
        max_col = std::max(max_col, right_column(curr));
        m_category = m_category | s.category();
      }

      m_rect = Rect(Point(min_line, min_col), Point(max_line, max_col));
    }

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return m_rect;
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(envelopes(rect(), cursor));

      // TODO: improve time complexity

      Draw_Result result {};
      for(Sprite const& sprite : m_sprites) {
        if(!trim::envelopes(sprite.rect(), cursor))
          continue;

        Draw_Result curr = sprite.draw(style, cursor);
        if(curr.character != "")
          result = curr;
      }

      return result;
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return m_category;
    }
  };

  static_assert(Is_Sprite<Composite_Sprite>);
} // namespace trim
