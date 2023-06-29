#pragma once
#include <trim/sprite/sprite.hpp>
#include <trim/style/style.hpp>
#include <trim/util/geometry.hpp>
#include <trim/util/ints.hpp>

#include <string>
#include <string_view>

namespace trim
{
  struct Joint_Sprite
  {
    private:

    Multi_Joint m_joint {};

    public:

    Joint_Sprite() = default;

    constexpr Joint_Sprite(Multi_Joint joint) noexcept
      : m_joint(joint)
    {}

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return Rect(Point::origin, Point::origin);
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      TRIM_ASSERT(trim::envelopes(rect(), cursor));

      switch(m_joint) {
        case Multi_Joint::RIGHT_DOWN: return Draw_Result {style.joint_right_down, style.branch_color};
        case Multi_Joint::RIGHT_UP: return Draw_Result {style.joint_right_up, style.branch_color};
        case Multi_Joint::DOWN_LEFT: return Draw_Result {style.joint_down_left, style.branch_color};
        case Multi_Joint::LEFT_UP: return Draw_Result {style.joint_left_up, style.branch_color};
        case Multi_Joint::RIGHT_DOWN_LEFT: return Draw_Result {style.joint_right_down_left, style.branch_color};
        case Multi_Joint::RIGHT_DOWN_UP: return Draw_Result {style.joint_right_down_up, style.branch_color};
        case Multi_Joint::RIGHT_LEFT_UP: return Draw_Result {style.joint_right_left_up, style.branch_color};
        case Multi_Joint::DOWN_LEFT_UP: return Draw_Result {style.joint_down_left_up, style.branch_color};
        case Multi_Joint::ALL: return Draw_Result {style.joint_all, style.branch_color};
      }

      TRIM_ASSERT(false);
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return Sprite_Category::BRANCH;
    }
  };

  static_assert(Is_Sprite<Joint_Sprite>);
} // namespace trim
