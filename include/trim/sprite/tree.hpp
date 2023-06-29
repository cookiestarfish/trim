#pragma once
#include <trim/sprite/composite.hpp>
#include <trim/sprite/joint.hpp>
#include <trim/sprite/line.hpp>
#include <trim/sprite/node.hpp>
#include <trim/sprite/spline3.hpp>
#include <trim/sprite/sprite.hpp>
#include <trim/sprite/transform.hpp>
#include <trim/style/style.hpp>
#include <trim/util/geometry.hpp>

#include <trim/container/labels.hpp>
#include <trim/container/tree.hpp>
#include <trim/layout/tree_layout.hpp>

namespace trim
{
  struct Tree_Sprite
  {
    private:

    Composite_Sprite m_composite {};

    public:

    Tree_Sprite() = default;

    constexpr Tree_Sprite(Tree const& tree, size_type root, Labels const& node_labels, [[maybe_unused]] Labels const& edge_labels, Tree_Layout const& layout)
      : m_composite()
    {
      size_type const num_nodes = tree.size();

      std::vector<Sprite> sprites {};

      // add all node sprites
      for(size_type node = 0; node < num_nodes; ++node) {
        Rect const rect = layout[node].rect;
        coord_type const node_height = height(rect);
        coord_type const node_width = width(rect);
        bool const is_top_connected = (node != root);
        bool const is_bot_connected = (tree.num_children(node) > 0);
        TRIM_ASSERT(node_height > 0);
        TRIM_ASSERT(node_width > 0);
        Sprite sprite = Node_Sprite(node_height, node_width, is_top_connected, is_bot_connected, node_labels(node));
        sprites.push_back(Translate_Sprite(std::move(sprite), rect.p1.line, rect.p1.column));
      }

      auto const add_direct_branch = [&](size_type node, size_type child) -> void {
        Rect const parent_rect = layout[node].rect;
        Rect const child_rect = layout[child].rect;

        Point const parent_point = trim::midpoint(trim::bot_segment(parent_rect));
        Point const child_point = trim::midpoint(trim::top_segment(child_rect));
        TRIM_ASSERT(parent_point.column == child_point.column);

        Sprite line = Vertical_Line_Sprite(child_point.line - parent_point.line, true, true);
        sprites.push_back(Translate_Sprite(std::move(line), parent_point.line, parent_point.column));
      };

      auto const add_spline_branch = [&](size_type node, size_type child) -> void {
        Rect const parent_rect = layout[node].rect;
        Rect const child_rect = layout[child].rect;

        Point const parent_point = trim::midpoint(trim::bot_segment(parent_rect));
        Point const child_point = trim::midpoint(trim::top_segment(child_rect));
        TRIM_ASSERT(parent_point.column != child_point.column);

        Point const mid1 = Point(std::midpoint(parent_point.line, child_point.line), parent_point.column);
        Point const mid2 = Point(mid1.line, child_point.column);

        // translate the spline anchors to make parent_point the origin
        Point const r1 = trim::translate(mid1, -parent_point.line, -parent_point.column);
        Point const r2 = trim::translate(mid2, -parent_point.line, -parent_point.column);
        Point const r3 = trim::translate(child_point, -parent_point.line, -parent_point.column);
        Sprite spline = Spline3_Sprite(r1, r2, r3, true, true);
        sprites.push_back(Translate_Sprite(std::move(spline), parent_point.line, parent_point.column));
      };

      auto const add_trunk = [&](size_type node) -> void {
        size_type const num_children = tree.num_children(node);
        size_type const leftmost = tree.get_child(node, 0);
        size_type const rightmost = tree.get_child(node, num_children - 1);

        Rect const parent_rect = layout[node].rect;
        Rect const left_rect = layout[leftmost].rect;
        Rect const right_rect = layout[rightmost].rect;

        Point const parent_point = trim::midpoint(trim::bot_segment(parent_rect));
        Point const left_point = trim::midpoint(trim::top_segment(left_rect));
        Point const right_point = trim::midpoint(trim::top_segment(right_rect));

        Point const trunk_down = Point(std::midpoint(parent_point.line, left_point.line), parent_point.column);
        Point const trunk_left = Point(trunk_down.line, left_point.column);
        Point const trunk_right = Point(trunk_down.line, right_point.column);

        // draw the horizontal trunk line
        coord_type const trunk_length = trim::signed_length(Horizontal_Segment(trunk_left, trunk_right));
        Sprite trunk_hline = Horizontal_Line_Sprite(trunk_length, true, true);
        sprites.push_back(Translate_Sprite(std::move(trunk_hline), trunk_left.line, trunk_left.column));

        // draw the vertical trunk line starting from parent
        coord_type const trunk_height = trim::signed_length(Vertical_Segment(parent_point, trunk_down));
        Sprite trunk_vline = Vertical_Line_Sprite(trunk_height, true, true);
        sprites.push_back(Translate_Sprite(std::move(trunk_vline), parent_point.line, parent_point.column));

        // draw the down trunk joint
        if(trunk_down == trunk_left) {
          Sprite joint = Joint_Sprite(Multi_Joint::RIGHT_DOWN_UP);
          sprites.push_back(Translate_Sprite(std::move(joint), trunk_down.line, trunk_down.column));
        } else if(trunk_down == trunk_right) {
          Sprite joint = Joint_Sprite(Multi_Joint::DOWN_LEFT_UP);
          sprites.push_back(Translate_Sprite(std::move(joint), trunk_down.line, trunk_down.column));
        } else if(trunk_down.column > trunk_right.column) {
          Sprite joint = Joint_Sprite(Multi_Joint::LEFT_UP);
          sprites.push_back(Translate_Sprite(std::move(joint), trunk_down.line, trunk_down.column));
        } else if(trunk_down.column < trunk_left.column) {
          Sprite joint = Joint_Sprite(Multi_Joint::RIGHT_UP);
          sprites.push_back(Translate_Sprite(std::move(joint), trunk_down.line, trunk_down.column));
        } else {
          Sprite joint = Joint_Sprite(Multi_Joint::RIGHT_LEFT_UP);
          sprites.push_back(Translate_Sprite(std::move(joint), trunk_down.line, trunk_down.column));
        }

        // draw the trunk left corner
        if(trunk_left != trunk_down) {
          if(trunk_left.column < trunk_down.column) {
            Sprite corner = Joint_Sprite(Multi_Joint::RIGHT_DOWN);
            sprites.push_back(Translate_Sprite(std::move(corner), trunk_left.line, trunk_left.column));
          } else {
            Sprite corner = Joint_Sprite(Multi_Joint::RIGHT_DOWN_LEFT);
            sprites.push_back(Translate_Sprite(std::move(corner), trunk_left.line, trunk_left.column));
          }
        }

        // draw the trunk right corner
        if(trunk_right != trunk_down) {
          if(trunk_right.column > trunk_down.column) {
            Sprite corner = Joint_Sprite(Multi_Joint::DOWN_LEFT);
            sprites.push_back(Translate_Sprite(std::move(corner), trunk_right.line, trunk_right.column));
          } else {
            Sprite corner = Joint_Sprite(Multi_Joint::RIGHT_DOWN_LEFT);
            sprites.push_back(Translate_Sprite(std::move(corner), trunk_right.line, trunk_right.column));
          }
        }

        // if trunk_down is before trunk_left, draw an additional horizontal segment
        if(trunk_down.column < trunk_left.column) {
          Sprite line = Horizontal_Line_Sprite(trunk_left.column - trunk_down.column, true, true);
          sprites.push_back(Translate_Sprite(std::move(line), trunk_down.line, trunk_down.column));
        }

        // if trunk_down is after trunk_right, draw an additional horizontal segment
        if(trunk_down.column > trunk_right.column) {
          Sprite line = Horizontal_Line_Sprite(trunk_right.column - trunk_down.column, true, true);
          sprites.push_back(Translate_Sprite(std::move(line), trunk_right.line, trunk_right.column));
        }

        // draw the inner trunk joints
        for(size_type i = 1; i + 1 < num_children; ++i) {
          size_type const child = tree.get_child(node, i);
          Rect const child_rect = layout[child].rect;
          Point const child_point = trim::midpoint(trim::top_segment(child_rect));
          Point const trunk_point = Point(trunk_down.line, child_point.column);

          if(trunk_point == trunk_down) {
            Sprite joint = Joint_Sprite(Multi_Joint::ALL);
            sprites.push_back(Translate_Sprite(std::move(joint), trunk_point.line, trunk_point.column));
          } else {
            Sprite joint = Joint_Sprite(Multi_Joint::RIGHT_DOWN_LEFT);
            sprites.push_back(Translate_Sprite(std::move(joint), trunk_point.line, trunk_point.column));
          }
        }

        // draw the vertical branches if needed
        for(size_type i = 0; i < num_children; ++i) {
          size_type const child = tree.get_child(node, i);
          Rect const child_rect = layout[child].rect;
          Point const child_point = trim::midpoint(trim::top_segment(child_rect));
          Point const trunk_point = Point(trunk_down.line, child_point.column);
          coord_type dist = trim::length(Vertical_Segment(trunk_point, child_point));
          if(dist > 1) {
            Sprite line = Vertical_Line_Sprite(dist, true, true);
            sprites.push_back(Translate_Sprite(std::move(line), trunk_point.line, trunk_point.column));
          }
        }
      };

      // add all single branches
      for(size_type node = 0; node < num_nodes; ++node) {
        if(tree.num_children(node) == 1) {
          size_type const child = tree.get_child(node, 0);

          Rect const parent_rect = layout[node].rect;
          Point const parent_point = trim::midpoint(trim::bot_segment(parent_rect));

          Rect const child_rect = layout[child].rect;
          Point const child_point = trim::midpoint(trim::top_segment(child_rect));

          if(parent_point.column == child_point.column) {
            add_direct_branch(node, child);
          } else {
            add_spline_branch(node, child);
          }
        } else if(tree.num_children(node) > 1) {
          add_trunk(node);
        }
      }

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
      return Sprite_Category::NODE | Sprite_Category::BRANCH | Sprite_Category::TEXT;
    }
  };

  static_assert(Is_Sprite<Tree_Sprite>);
} // namespace trim
