#pragma once
#include <trim/color/rgb.hpp>
#include <trim/util/ints.hpp>

#include <string_view>

namespace trim
{
  enum class Text_Alignment
  {
    NONE = 0,
    LEFT = 1,
    CENTER = 2,
    RIGHT = 3
  };

  // clang-format off

  enum class Text_Modifier
  {
    NONE      = 0b000,
    BOLD      = 0b001,
    UNDERLINE = 0b010,
    ITALIC    = 0b100
  };

  // clang-format on

  enum class Tree_Alignment
  {
    NONE = 0,
    LEFT = 1,
    CENTER = 2,
    RIGHT = 3
  };

  enum class Trit
  {
    NONE = 0,
    FALSE = 1,
    TRUE = 2
  };

  enum class Single_Joint
  {
    RIGHT = 0b0001,
    DOWN = 0b0010,
    LEFT = 0b0100,
    UP = 0b1000
  };

  enum class Multi_Joint
  {
    RIGHT_DOWN = 0b0011,
    RIGHT_UP = 0b1001,
    DOWN_LEFT = 0b0110,
    LEFT_UP = 0b1100,

    RIGHT_DOWN_LEFT = 0b0111,
    RIGHT_DOWN_UP = 0b1011,
    RIGHT_LEFT_UP = 0b1101,
    DOWN_LEFT_UP = 0b1110,

    ALL = 0b1111
  };

  [[nodiscard]] constexpr Text_Modifier operator|(Text_Modifier lhs, Text_Modifier rhs) noexcept
  {
    return Text_Modifier(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
  }

  [[nodiscard]] constexpr Multi_Joint operator|(Single_Joint lhs, Single_Joint rhs) noexcept
  {
    return Multi_Joint(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
  }

  [[nodiscard]] constexpr Multi_Joint operator|(Multi_Joint lhs, Single_Joint rhs) noexcept
  {
    return Multi_Joint(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
  }

  [[nodiscard]] constexpr Multi_Joint operator|(Single_Joint lhs, Multi_Joint rhs) noexcept
  {
    return Multi_Joint(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
  }

  [[nodiscard]] constexpr Multi_Joint operator|(Multi_Joint lhs, Multi_Joint rhs) noexcept
  {
    return Multi_Joint(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
  }

  // clang-format off

  struct Style
  {
    std::string_view box_vertical_line    = "|";
    std::string_view box_horizontal_line  = "─";
    std::string_view box_top_left_corner  = "┌";
    std::string_view box_top_right_corner = "┐";
    std::string_view box_bot_left_corner  = "└";
    std::string_view box_bot_right_corner = "┘";

    std::string_view vertical_line        = "|";
    std::string_view horizontal_line      = "─";

    std::string_view top_connection       = "┴";
    std::string_view bot_connection       = "┬";

    std::string_view joint_down_left      = "┐";
    std::string_view joint_right_down     = "┌";
    std::string_view joint_left_up        = "┘";
    std::string_view joint_right_up       = "└";

    std::string_view joint_right_down_left = "┬";
    std::string_view joint_right_down_up   = "├";
    std::string_view joint_right_left_up   = "┴";
    std::string_view joint_down_left_up    = "┤";
    std::string_view joint_all             = "┼";

    // Color_RGB::NONE is a magic constant that means 'no color'
    Color_RGB box_color                   = Color_RGB::NONE;
    Color_RGB branch_color                = Color_RGB::NONE;
    Color_RGB text_color                  = Color_RGB::NONE;

    Text_Modifier text_modifier           = Text_Modifier::NONE;
    Text_Alignment text_align             = Text_Alignment::NONE;

    ssize_type sibling_margin             = -1;
    ssize_type level_margin               = -1;
    ssize_type node_vertical_padding      = -1;
    ssize_type node_horizontal_padding    = -1;
    ssize_type node_minimum_width         = -1;
    ssize_type node_minimum_height        = -1;

    Tree_Alignment tree_align = Tree_Alignment::NONE;

    bool operator==(Style const&) const = default;
  };

  constexpr inline Style thin_style = {
    .box_vertical_line    = "|",
    .box_horizontal_line  = "─",
    .box_top_left_corner  = "┌",
    .box_top_right_corner = "┐",
    .box_bot_left_corner  = "└",
    .box_bot_right_corner = "┘",

    .vertical_line    = "|",
    .horizontal_line  = "─",

    .top_connection       = "┴",
    .bot_connection       = "┬",

    .joint_down_left    = "┐",
    .joint_right_down   = "┌",
    .joint_left_up      = "┘",
    .joint_right_up     = "└",

    .joint_right_down_left  = "┬",
    .joint_right_down_up    = "├",
    .joint_right_left_up    = "┴",
    .joint_down_left_up     = "┤",
    .joint_all              = "┼",

    .box_color        = Color_RGB::NONE,
    .branch_color     = Color_RGB::NONE,
    .text_color       = Color_RGB::NONE,
    .text_modifier    = Text_Modifier::NONE,
    .text_align   = Text_Alignment::CENTER,

    .sibling_margin           = 2,
    .level_margin             = 1,
    .node_vertical_padding    = 0,
    .node_horizontal_padding  = 1,
    .node_minimum_width       = 0,
    .node_minimum_height      = 0,

    .tree_align = Tree_Alignment::CENTER
  };

  constexpr inline Style default_style = thin_style;

  constexpr inline Style thick_style = {
    .box_vertical_line    = "┃",
    .box_horizontal_line  = "━",
    .box_top_left_corner  = "┏",
    .box_top_right_corner = "┓",
    .box_bot_left_corner  = "┗",
    .box_bot_right_corner = "┛",

    .vertical_line    = "┃",
    .horizontal_line  = "━",

    .top_connection       = "┻",
    .bot_connection       = "┳",

    .joint_down_left    = "┓",
    .joint_right_down   = "┏",
    .joint_left_up      = "┛",
    .joint_right_up     = "┗",

    .joint_right_down_left  = "┳",
    .joint_right_down_up    = "┣",
    .joint_right_left_up    = "┻",
    .joint_down_left_up     = "┫",
    .joint_all              = "╋",

    .box_color     = default_style.box_color,
    .branch_color  = default_style.branch_color,
    .text_color    = default_style.text_color,
    .text_modifier = default_style.text_modifier,
    .text_align    = default_style.text_align,

    .sibling_margin           = default_style.sibling_margin,
    .level_margin             = default_style.level_margin,
    .node_vertical_padding    = default_style.node_vertical_padding,
    .node_horizontal_padding  = default_style.node_horizontal_padding,
    .node_minimum_width       = default_style.node_minimum_width,
    .node_minimum_height      = default_style.node_minimum_height,
    
    .tree_align = default_style.tree_align,
  };

  // clang-format on

  // clang-format off

  constexpr inline Style double_style = {
    .box_vertical_line    = "║",
    .box_horizontal_line  = "═",
    .box_top_left_corner  = "╔",
    .box_top_right_corner = "╗",
    .box_bot_left_corner  = "╚",
    .box_bot_right_corner = "╝",

    .vertical_line    = "║",
    .horizontal_line  = "═",

    .top_connection       = "╩",
    .bot_connection       = "╦",

    .joint_down_left    = "╗",
    .joint_right_down   = "╔",
    .joint_left_up      = "╝",
    .joint_right_up     = "╚",

    .joint_right_down_left  = "╦",
    .joint_right_down_up    = "╠",
    .joint_right_left_up    = "╩",
    .joint_down_left_up     = "╣",
    .joint_all              = "╬",

    .box_color      = default_style.box_color,
    .branch_color   = default_style.branch_color,
    .text_color     = default_style.text_color,
    .text_modifier  = default_style.text_modifier,
    .text_align     = default_style.text_align,

    .sibling_margin           = default_style.sibling_margin,
    .level_margin             = default_style.level_margin,
    .node_vertical_padding    = default_style.node_vertical_padding,
    .node_horizontal_padding  = default_style.node_horizontal_padding,
    .node_minimum_width       = default_style.node_minimum_width,
    .node_minimum_height      = default_style.node_minimum_height,

    .tree_align = default_style.tree_align,
  };

  // clang-format on

} // namespace trim
