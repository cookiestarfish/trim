#pragma once
#include <trim/sprite/character.hpp>
#include <trim/sprite/composite.hpp>
#include <trim/sprite/joint.hpp>
#include <trim/sprite/line.hpp>
#include <trim/sprite/sprite.hpp>
#include <trim/sprite/transform.hpp>
#include <trim/style/style.hpp>
#include <trim/util/geometry.hpp>

namespace trim
{
  struct Spline3_Sprite
  {
    private:

    Composite_Sprite m_composite {};

    public:

    constexpr Spline3_Sprite(Point p1, Point p2, Point p3, bool ignore_start = 0, bool ignore_end = 0) noexcept
      : m_composite()
    {
      TRIM_ASSERT(p1.line == 0 || p1.column == 0);
      TRIM_ASSERT(p2.line == p1.line || p2.column == p1.column);
      TRIM_ASSERT(p3.line == p2.line || p3.column == p2.column);
      TRIM_ASSERT(p1 != p2);
      TRIM_ASSERT(p2 != p3);

      std::vector<Sprite> sprites {};

      // returns either a vertical line or a horizontal line
      auto const make_line_sprite = [](Point p, bool ignore_start, bool ignore_end) -> Sprite {
        switch(trim::axis(Point::origin, p)) {
          case Axis::HORIZONTAL: return Horizontal_Line_Sprite(p.column, ignore_start, ignore_end);
          case Axis::VERTICAL: return Vertical_Line_Sprite(p.line, ignore_start, ignore_end);
        }

        TRIM_ASSERT(false);
      };

      // returns the joint for p0->p1->p2 .
      auto const get_join_character = [](Point p0, Point p1, Point p2) -> Sprite {
        Direction dir1 = trim::direction(p0, p1);
        Direction dir2 = trim::direction(p1, p2);

        bool has_down = (dir1 == Direction::UP) || (dir2 == Direction::DOWN);
        bool has_right = (dir1 == Direction::LEFT) || (dir2 == Direction::RIGHT);

        Single_Joint j1 = has_down ? Single_Joint::DOWN : Single_Joint::UP;
        Single_Joint j2 = has_right ? Single_Joint::RIGHT : Single_Joint::LEFT;
        return Joint_Sprite(j1 | j2);
      };

      Sprite l1 = make_line_sprite(p1, ignore_start, true);
      Sprite l2 = make_line_sprite(trim::translate(p2, -p1.line, -p1.column), true, true);
      Sprite l3 = make_line_sprite(trim::translate(p3, -p2.line, -p2.column), true, ignore_end);
      Sprite join1 = get_join_character(Point::origin, p1, p2);
      Sprite join2 = get_join_character(p1, p2, p3);
      sprites.push_back(std::move(l1));
      sprites.push_back(Translate_Sprite(std::move(l2), p1.line, p1.column));
      sprites.push_back(Translate_Sprite(std::move(l3), p2.line, p2.column));
      sprites.push_back(Translate_Sprite(std::move(join1), p1.line, p1.column));
      sprites.push_back(Translate_Sprite(std::move(join2), p2.line, p2.column));
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
      return Sprite_Category::BRANCH;
    }
  };

  static_assert(Is_Sprite<Spline3_Sprite>);
} // namespace trim
