#pragma once
#include <trim/cli/option.hpp>
#include <trim/cli/parse.hpp>
#include <trim/color/parse.hpp>
#include <trim/color/rgb.hpp>
#include <trim/style/style.hpp>

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace trim::cli
{
  struct Options
  {
    std::optional<std::string_view> input_file_name {};
    std::optional<Style> style {};
    std::optional<std::string_view> positional_input {};

    bool print_help {};

    std::optional<Tree_Alignment> tree_align {};
    std::optional<Text_Modifier> text_modifier {};
    std::optional<Text_Alignment> text_align {};
    std::optional<Color_RGB> text_color {};
    std::optional<Color_RGB> box_color {};
    std::optional<Color_RGB> branch_color {};

    std::optional<int> level_margin {};
    std::optional<int> sibling_margin {};
    std::optional<int> horizontal_padding {};
    std::optional<int> vertical_padding {};
    std::optional<int> node_min_width {};
    std::optional<int> node_min_height {};

    std::vector<std::string> errors {};
  };

  [[nodiscard]] constexpr bool is_boolean(std::string_view string) noexcept
  {
    if(string == "true")
      return true;
    if(string == "false")
      return true;
    if(string == "0")
      return true;
    if(string == "1")
      return true;
    return false;
  }

  [[nodiscard]] constexpr bool is_small_positive_integer(std::string_view string) noexcept
  {
    for(char x : string)
      if(x < '0' || x > '9')
        return false;
    if(string.size() > 3)
      return false;
    return true;
  }

  [[nodiscard]] constexpr bool is_color(std::string_view string) noexcept
  {
    if(auto test = trim::parse_color_hex(string); test)
      return true;
    if(auto test = trim::parse_color_name(string); test)
      return true;
    return false;
  }

  [[nodiscard]] constexpr bool is_text_alignment(std::string_view string) noexcept
  {
    if(string == "left")
      return true;
    if(string == "right")
      return true;
    if(string == "center")
      return true;
    return false;
  }

  [[nodiscard]] constexpr bool is_tree_alignment(std::string_view string) noexcept
  {
    if(string == "left")
      return true;
    if(string == "right")
      return true;
    if(string == "center")
      return true;
    return false;
  }

  [[nodiscard]] constexpr bool is_style(std::string_view string) noexcept
  {
    if(string == "default")
      return true;
    if(string == "thick")
      return true;
    if(string == "thin")
      return true;
    if(string == "double")
      return true;
    return false;
  }

  [[nodiscard]] constexpr bool is_text_modifier(std::string_view string) noexcept
  {
    if(string == "bold")
      return true;
    if(string == "underline")
      return true;
    if(string == "italic")
      return true;
    return false;
  }

  [[nodiscard]] constexpr bool to_boolean(std::string_view string) noexcept
  {
    TRIM_ASSERT(is_boolean(string));

    if(string == "true" || string == "1")
      return true;
    if(string == "false" || string == "0")
      return false;

    TRIM_ASSERT(false);
  }

  [[nodiscard]] constexpr int to_small_positive_integer(std::string_view string) noexcept
  {
    TRIM_ASSERT(is_small_positive_integer(string));

    int result = 0;
    for(char c : string)
      result = result * 10 + c - '0';
    return result;
  }

  [[nodiscard]] constexpr Color_RGB to_color(std::string_view string) noexcept
  {
    TRIM_ASSERT(is_color(string));

    if(auto color = trim::parse_color_hex(string); color)
      return color.value();

    if(auto color = trim::parse_color_name(string); color)
      return color.value();

    TRIM_ASSERT(false);
  }

  [[nodiscard]] constexpr Style to_style(std::string_view string) noexcept
  {
    TRIM_ASSERT(is_style(string));

    if(string == "default")
      return trim::default_style;
    if(string == "thin")
      return trim::default_style;
    if(string == "thick")
      return trim::thick_style;
    if(string == "double")
      return trim::double_style;

    TRIM_ASSERT(false);
  }

  [[nodiscard]] constexpr Text_Alignment to_text_alignment(std::string_view string) noexcept
  {
    TRIM_ASSERT(is_text_alignment(string));

    if(string == "left")
      return Text_Alignment::LEFT;
    if(string == "center")
      return Text_Alignment::CENTER;
    if(string == "right")
      return Text_Alignment::RIGHT;

    TRIM_ASSERT(false);
  }

  [[nodiscard]] constexpr Tree_Alignment to_tree_alignment(std::string_view string) noexcept
  {
    TRIM_ASSERT(is_tree_alignment(string));

    if(string == "left")
      return Tree_Alignment::LEFT;
    if(string == "center")
      return Tree_Alignment::CENTER;
    if(string == "right")
      return Tree_Alignment::RIGHT;

    TRIM_ASSERT(false);
  }

  [[nodiscard]] constexpr Text_Modifier to_text_modifier(std::string_view string) noexcept
  {
    TRIM_ASSERT(is_text_modifier(string));

    if(string == "bold")
      return Text_Modifier::BOLD;
    if(string == "italic")
      return Text_Modifier::ITALIC;
    if(string == "underline")
      return Text_Modifier::UNDERLINE;

    TRIM_ASSERT(false);
  }

  [[nodiscard]] constexpr Options parse_arguments(std::span<char const* const> args)
  {
    Parse_Result const parsed = parse(args);
    Options result {};

    for(Option const& option : parsed.parsed_arguments()) {
      if(auto maybe_name = option.name(); maybe_name.has_value()) {
        std::string_view name = maybe_name.value();
        auto maybe_value = option.value();

        if(name == "h" || name == "help") {
          result.print_help = true;
          continue;
        }

        if(name == "i" || name == "input" || name == "input-file") {
          if(!maybe_value.has_value()) {
            result.errors.push_back("Incorrect usage of 'input'. Expected file name.");
          } else {
            result.input_file_name = maybe_value.value();
          }
          continue;
        }

        if(name == "style") {
          if(maybe_value.has_value() && is_style(*maybe_value)) {
            result.style = to_style(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'style'. Expected a style name.");
          }
          continue;
        }

        if(name == "text-color") {
          if(maybe_value.has_value() && is_color(*maybe_value)) {
            result.text_color = to_color(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'text-color'. Expected a color name or hex value.");
          }
          continue;
        }

        if(name == "box-color") {
          if(maybe_value.has_value() && is_color(*maybe_value)) {
            result.box_color = to_color(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'box-color'. Expected a color name or hex value.");
          }
          continue;
        }

        if(name == "branch-color") {
          if(maybe_value.has_value() && is_color(*maybe_value)) {
            result.branch_color = to_color(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'branch-color'. Expected a color name or hex value.");
          }
          continue;
        }

        if(name == "tree-align") {
          if(maybe_value.has_value() && is_tree_alignment(*maybe_value)) {
            result.tree_align = to_tree_alignment(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'tree-align'. Expected left|right|center.");
          }
          continue;
        }

        if(name == "text-align") {
          if(maybe_value.has_value() && is_text_alignment(*maybe_value)) {
            result.text_align = to_text_alignment(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'text-align'. Expected left|right|center.");
          }
          continue;
        }

        if(name == "level-margin") {
          if(maybe_value.has_value() && is_small_positive_integer(*maybe_value)) {
            result.level_margin = to_small_positive_integer(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'level-margin'. Expected a small positive integer.");
          }
          continue;
        }

        if(name == "sibling-margin") {
          if(maybe_value.has_value() && is_small_positive_integer(*maybe_value)) {
            result.sibling_margin = to_small_positive_integer(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'sibling-margin'. Expected a small positive integer.");
          }
          continue;
        }

        if(name == "vertical-padding") {
          if(maybe_value.has_value() && is_small_positive_integer(*maybe_value)) {
            result.vertical_padding = to_small_positive_integer(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'vertical-padding'. Expected a small positive integer.");
          }
          continue;
        }

        if(name == "horizontal-padding") {
          if(maybe_value.has_value() && is_small_positive_integer(*maybe_value)) {
            result.horizontal_padding = to_small_positive_integer(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'horizontal-padding'. Expected a small positive integer.");
          }
          continue;
        }

        if(name == "node-min-width") {
          if(maybe_value.has_value() && is_small_positive_integer(*maybe_value)) {
            result.node_min_width = to_small_positive_integer(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'node-min-width'. Expected a small positive integer.");
          }
          continue;
        }

        if(name == "node-min-height") {
          if(maybe_value.has_value() && is_small_positive_integer(*maybe_value)) {
            result.node_min_height = to_small_positive_integer(*maybe_value);
          } else {
            result.errors.push_back("Incorrect usage of 'node-min-height'. Expected a small positive integer.");
          }
          continue;
        }

        result.errors.push_back("Unrecognized argument " + std::string(name));
      } else {
        result.positional_input = option.value().value();
      }
    }

    for(std::string_view unparsed_argument : parsed.unparsed_arguments()) {
      result.errors.push_back("Unparsed argument '" + std::string(unparsed_argument) + "'");
    }

    return result;
  }

  [[nodiscard]] constexpr std::string_view help_message() noexcept
  {
    return R"EOF(
Usage: trim [--input=file_name] [--style=styleName] [--style-parameter=value] [--help]
  --input               | read from the given file instead of stdin
  --style               | configure a style (default, thin, thick, double)
  --tree-align          | configure tree alignment (left, center, right)
  --text-style          | configure the text style (bold, italic, underline)
  --text-align          | enable label alignment (left, center, right)
  --text-color          | configure text color
  --box-color           | configure box color 
  --branch-color        | configure branch color 
  --line-margin         | configure vertical margin between parent and child nodes
  --sibling-margin      | configure horizontal margin between sibling nodes
  --horizontal-padding  | configure horizontal label padding
  --vertical-padding    | configure vertical label padding
)EOF";
  }
} // namespace trim::cli
