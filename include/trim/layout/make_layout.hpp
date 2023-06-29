#pragma once
#include <trim/container/contour.hpp>
#include <trim/container/labels.hpp>
#include <trim/container/tree.hpp>
#include <trim/layout/tree_layout.hpp>
#include <trim/style/style.hpp>
#include <trim/util/ints.hpp>
#include <trim/util/split.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace trim
{
  constexpr Tree_Layout make_layout(            //
    Tree const& tree,                           //
    size_type root,                             //
    Labels const& node_labels,                  //
    [[maybe_unused]] Labels const& edge_labels, //
    Style const& style)
  {
    size_type const N = tree.size();

    Tree_Layout result = Tree_Layout(N);
    std::vector<ssize_type> levels = std::vector<ssize_type>(N, 0);
    std::vector<ssize_type> max_level_height = std::vector<ssize_type>(N, 0);
    std::vector<ssize_type> max_level_margin = std::vector<ssize_type>(N, 0);
    std::vector<ssize_type> lines = std::vector<ssize_type>(N, 0);
    std::vector<ssize_type> offsets = std::vector<ssize_type>(N, 0);
    std::vector<ssize_type> node_width = std::vector<ssize_type>(N, 0);
    std::vector<ssize_type> node_height = std::vector<ssize_type>(N, 0);
    std::vector<Contour> left_contours = std::vector<Contour>(N);
    std::vector<Contour> right_contours = std::vector<Contour>(N);

    // returns the precomputed width of a node
    auto const width_map = [&node_width](size_type node) -> ssize_type {
      return node_width[node];
    };

    // split a string into lines and compute the maximum length of any line
    auto const max_text_line_length = [&](std::string_view string) -> size_type {
      size_type max_length = 0;
      trim::split_string_by_newline(string, [&](std::string_view line) {
        max_length = std::max(max_length, line.size());
      });
      return max_length;
    };

    // compute the level of each node
    // the level is the distance from root
    trim::tree_compute_levels(tree, root, levels);

    // compute width and height of every node
    for(size_type node = 0; node < N; ++node) {
      ssize_type const text_length = std::max(max_text_line_length(node_labels(node)), size_type(1));
      ssize_type const text_lines = std::ranges::count(node_labels(node), '\n') + 1;
      ssize_type const h_padding = style.node_horizontal_padding * 2;
      ssize_type const v_padding = style.node_vertical_padding * 2;
      node_width[node] = std::max(style.node_minimum_width, text_length + h_padding + 2);
      node_height[node] = std::max(style.node_minimum_height, text_lines + v_padding + 2);

      // round up the node width so that the connection points are exactly centered
      if(node_width[node] % 2 == 0)
        node_width[node] += 1;
    }

    // for each level, compute the maximum node height and vertical margin on that level
    for(size_type node = 0; node < N; ++node) {
      ssize_type const level = levels[node];
      ssize_type const height = node_height[node];
      ssize_type const margin = style.level_margin;
      max_level_height[level] = std::max(max_level_height[level], height);
      max_level_margin[level] = std::max(max_level_margin[level], margin);
    }

    // compute the line of every node
    // the line of a node is the y coordinate of its enclosing box top segment
    trim::tree_visit_preorder(tree, root, [&](size_type curr) -> void {
      for(size_type i = 0; i < tree.num_children(curr); ++i) {
        size_type child = tree.get_child(curr, i);
        lines[child] = lines[curr] + max_level_height[levels[curr]] + max_level_margin[levels[curr]];
      }
    });

    // compute offsets and contours
    trim::tree_visit_postorder(tree, root, [&](size_type curr) -> void {
      size_type const num_children = tree.num_children(curr);
      bool const is_leaf = (num_children == 0);
      bool const is_unary_node = (num_children == 1);
      bool const is_binary_node = (num_children == 2);

      if(is_leaf) {
        offsets[curr] = 0;
        left_contours[curr].push_back(curr, 0);
        right_contours[curr].push_back(curr, 0);
        return;
      }

      if(is_unary_node) {
        size_type child = tree.get_child(curr, 0);
        ssize_type w1 = width_map(curr);
        ssize_type w2 = width_map(child);

        if(style.tree_align == Tree_Alignment::CENTER) {
          if(w2 > w1) {
            offsets[child] = -(w2 - w1 + 1) / 2;
          } else if(w2 < w1) {
            offsets[child] = +(w1 - w2 + 1) / 2;
          }
        } else if(style.tree_align == Tree_Alignment::RIGHT) {
          if(w2 > w1) {
            offsets[child] = -(w2 - w1);
          } else if(w2 < w1) {
            offsets[child] = +(w1 - w2);
          }
        }

        left_contours[curr] = std::move(left_contours[child]);
        right_contours[curr] = std::move(right_contours[child]);
        left_contours[curr][0].offset = offsets[child];
        right_contours[curr][0].offset = offsets[child];
        left_contours[curr].push_front(curr, 0);
        right_contours[curr].push_front(curr, 0);

        return;
      }

      if(is_binary_node) {
        size_type left_child = tree.get_child(curr, 0);
        size_type right_child = tree.get_child(curr, 1);

        ssize_type offset = minimum_offset(right_contours[left_child], left_contours[right_child], width_map);
        offset += style.sibling_margin;
        ssize_type total_width = offset + width_map(right_child);
        ssize_type current_width = width_map(curr);

        ssize_type offset1 = 0;
        ssize_type offset2 = offset;

        switch(style.tree_align) {
          case Tree_Alignment::NONE: break;
          case Tree_Alignment::LEFT: break;
          case Tree_Alignment::CENTER: {
            offset1 = offset1 - (total_width / 2) + ((current_width + 1) / 2);
            offset2 = offset2 - (total_width - (total_width / 2)) + ((current_width + 1) / 2);
            break;
          }
          case Tree_Alignment::RIGHT: {
            offset1 = offset1 - total_width + current_width;
            offset2 = offset2 - total_width + current_width;
            break;
          }
        }
        offsets[curr] = 0;
        offsets[left_child] = offset1;
        offsets[right_child] = offset2;

        left_contours[curr] = std::move(left_contours[left_child]);
        right_contours[curr] = std::move(right_contours[right_child]);

        left_contours[curr][0].offset = offset1;
        right_contours[curr][0].offset = offset2;

        left_contours[curr].merge(std::move(left_contours[right_child]));
        right_contours[curr].merge(std::move(right_contours[left_child]));

        left_contours[curr].push_front(curr, 0);
        right_contours[curr].push_front(curr, 0);

        return;
      }

      // otherwise there are more than 2 children

      size_type leftmost_child = tree.get_child(curr, 0);
      size_type rightmost_child = tree.get_child(curr, num_children - 1);
      Contour previous_right_contour = std::move(right_contours[leftmost_child]);
      Contour previous_left_contour = std::move(left_contours[leftmost_child]);
      ssize_type total_width = 0;

      for(size_type i = 1; i < num_children; ++i) {
        size_type child = tree.get_child(curr, i);
        size_type prev_child = tree.get_child(curr, i - 1);
        left_contours[child][0].offset = offsets[prev_child];
        right_contours[child][0].offset = offsets[prev_child];

        ssize_type offset = minimum_offset(previous_right_contour, left_contours[child], width_map);
        offset += style.sibling_margin;

        right_contours[child][0].offset = offsets[prev_child] + offset;
        left_contours[child][0].offset = offsets[prev_child] + offset;

        right_contours[child].merge(std::move(previous_right_contour));
        previous_left_contour.merge(std::move(left_contours[child]));
        previous_right_contour = std::move(right_contours[child]);

        offsets[child] = offsets[prev_child] + offset;
        total_width = offsets[child] + width_map(child);
      }

      switch(style.tree_align) {
        case Tree_Alignment::NONE:
        case Tree_Alignment::LEFT: {
          break;
        }
        case Tree_Alignment::CENTER: {
          for(size_type i = 0; i < num_children; ++i) {
            size_type child = tree.get_child(curr, i);
            offsets[child] = offsets[child] - (total_width / 2) + (width_map(curr) / 2);
          }
          break;
        }
        case Tree_Alignment::RIGHT: {
          for(size_type i = 0; i < num_children; ++i) {
            size_type child = tree.get_child(curr, i);
            offsets[child] = offsets[child] - (total_width) + (width_map(curr));
          }
          break;
        }
      }

      left_contours[curr] = std::move(previous_left_contour);
      right_contours[curr] = std::move(previous_right_contour);
      left_contours[curr][0].offset = offsets[leftmost_child];
      right_contours[curr][0].offset = offsets[rightmost_child];

      left_contours[curr].push_front(curr, 0);
      right_contours[curr].push_front(curr, 0);
    });

    // Use the computed offsets to compute the final layout
    Point const root_top_left = Point(0, 0);
    Point const root_bot_right = Point(node_height[root] - 1, node_width[root] - 1);
    result[root] = Node_Layout(Rect(root_top_left, root_bot_right));

    trim::tree_visit_preorder(tree, root, [&](size_type curr) -> void {
      ssize_type subtree_width = 0;
      for(size_type i = 0; i < tree.num_children(curr); ++i) {
        size_type child = tree.get_child(curr, i);
        subtree_width = std::max(subtree_width, offsets[child] + node_width[child]);
      }

      Rect const parent_rect = result[curr].rect;
      coord_type const parent_left_column = left_column(parent_rect);

      for(size_type i = 0; i < tree.num_children(curr); ++i) {
        size_type child = tree.get_child(curr, i);
        coord_type const child_line = lines[child];
        coord_type const child_left_column = parent_left_column + offsets[child];
        coord_type const child_height = node_height[child];
        coord_type const child_width = node_width[child];
        Point const top_left = Point(child_line, child_left_column);
        Point const bot_right = Point(child_line + child_height - 1, child_left_column + child_width - 1);
        result[child] = Node_Layout(Rect(top_left, bot_right));
      }
    });

    return result;
  }
} // namespace trim
