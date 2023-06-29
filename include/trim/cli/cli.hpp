#pragma once
#include <trim/color/rgb.hpp>
#include <trim/style/style.hpp>
#include <trim/util/assert.hpp>

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace trim::cli
{
  struct Option
  {
    std::string_view name {};
    std::string_view value {};
    bool ok {};

    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
      return ok;
    }
  };

  struct Parser
  {
    std::span<char const* const> args {};
    bool positional_only {};

    explicit constexpr Parser(std::span<char const* const> args) noexcept
      : args(args)
      , positional_only(false)
    {}

    [[nodiscard]] constexpr std::string_view consume() & noexcept
    {
      TRIM_ASSERT(!args.empty());

      std::string_view result = args[0];
      args = args.subspan(1);
      return result;
    }

    [[nodiscard]] constexpr std::string_view peek() const noexcept
    {
      if(args.empty())
        return "";
      return args[0];
    }

    [[nodiscard]] constexpr bool done() const noexcept
    {
      return args.empty();
    }

    [[nodiscard]] constexpr Option next() & noexcept
    {
      if(args.empty()) {
        return Option {.ok = false};
      }

      if(positional_only) {
        return Option {.name = "", .value = consume(), .ok = true};
      }

      std::string_view string = consume();

      if(string == "--") {
        positional_only = true;
        if(!args.empty()) {
          return Option {.name = "", .value = consume(), .ok = true};
        } else {
          return Option {};
        }
      }

      if(string.starts_with("--")) {
        // try to parse a valued option with equal sign (--input=filename)
        // or a valued option without equal sign (--input filename)
        // or a flag (--flag)

        string.remove_prefix(2);
        std::size_t equal_pos = string.find('=');

        if(equal_pos == string.npos) {
          // if there is no equal sign
          // if there is no next option, return a flag
          // if the next option starts with "--", return a flag
          // otherwise consider the following string the value for this option
          if(args.empty() || peek().starts_with("--")) {
            return Option {.name = string, .value = "", .ok = true};
          } else {
            return Option {.name = string, .value = consume(), .ok = true};
          }
        } else {
          // otherwise split the string at the equal sign
          // the first part is the optioon name, the second part is the option value
          std::string_view name = string.substr(0, equal_pos);
          std::string_view value = string.substr(equal_pos + 1);
          return Option {.name = name, .value = value, .ok = true};
        }
      } else if(string.starts_with("-")) {
        string.remove_prefix(1);
        return Option {.name = string, .value = "", .ok = true};
      } else {
        return Option {.name = "", .value = string, .ok = true};
      }
    }
  };

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

  [[nodiscard]] constexpr std::optional<Style> parse_style(std::string_view string) noexcept
  {
    if(string == "thin")
      return trim::thin_style;
    if(string == "thick")
      return trim::thick_style;
    if(string == "double")
      return trim::double_style;
    return std::nullopt;
  }

  [[nodiscard]] constexpr std::optional<Tree_Alignment> parse_tree_align(std::string_view string) noexcept
  {
    if(string == "left")
      return Tree_Alignment::LEFT;
    if(string == "center")
      return Tree_Alignment::CENTER;
    if(string == "right")
      return Tree_Alignment::RIGHT;
    return std::nullopt;
  }

  [[nodiscard]] constexpr std::optional<Text_Alignment> parse_text_align(std::string_view string) noexcept
  {
    if(string == "left")
      return Text_Alignment::LEFT;
    if(string == "center")
      return Text_Alignment::CENTER;
    if(string == "right")
      return Text_Alignment::RIGHT;
    return std::nullopt;
  }

  [[nodiscard]] constexpr std::optional<Color_RGB> parse_color_hex(std::string_view hex) noexcept
  {
    auto const is_hex_digit = [](char character) noexcept -> bool {
      if(character >= 'a' && character <= 'f')
        return true;
      if(character >= 'A' && character <= 'F')
        return true;
      if(character >= '0' && character <= '9')
        return true;
      return false;
    };

    auto const digit_value = [](char character) noexcept -> int {
      if(character >= 'a' && character <= 'f')
        return character - 'a' + 10;
      if(character >= 'A' && character <= 'F')
        return character - 'A' + 10;
      if(character >= '0' && character <= '9')
        return character - '0';
      TRIM_ASSERT(false);
    };

    if(!hex.starts_with("#"))
      return std::nullopt;

    hex.remove_prefix(1);
    std::uint_least32_t value = 0;

    while(!hex.empty()) {
      int digit = hex.front();
      hex.remove_prefix(1);

      if(!is_hex_digit(digit))
        return std::nullopt;
      value = value * 16 + digit_value(digit);
    }

    return Color_RGB(value);
  }

  [[nodiscard]] constexpr std::optional<Color_RGB> parse_color_name(std::string_view name) noexcept
  {
    struct Color_Pair
    {
      std::string_view name;
      Color_RGB value;
    };

    // clang-format off
    constexpr Color_Pair colors[] = {
      Color_Pair {"none",               Color_RGB::NONE},
      Color_Pair {"red",                Color_RGB::RED},
      Color_Pair {"cherryred",          Color_RGB::CHERRY_RED},
      Color_Pair {"bubblegumpink",      Color_RGB::BUBBLEGUM_PINK},
      Color_Pair {"green",              Color_RGB::GREEN},
      Color_Pair {"blue",               Color_RGB::BLUE},
      Color_Pair {"black",              Color_RGB::BLACK},
      Color_Pair {"white",              Color_RGB::WHITE},
      Color_Pair {"gray",               Color_RGB::GRAY},
      Color_Pair {"pink",               Color_RGB::PINK},
      Color_Pair {"purple",             Color_RGB::PURPLE},
      Color_Pair {"cyan",               Color_RGB::CYAN},
      Color_Pair {"brown",              Color_RGB::BROWN},
      Color_Pair {"peach",              Color_RGB::PEACH},
      Color_Pair {"yellow",             Color_RGB::YELLOW},
      Color_Pair {"orange",             Color_RGB::ORANGE},
      Color_Pair {"burntorange",        Color_RGB::BURNT_ORANGE},
      Color_Pair {"rainbow",            Color_RGB::RAINBOW}
    };
    // clang-format on

    for(Color_Pair const& color : colors) {
      if(name == color.name) {
        return color.value;
      }
    }

    return std::nullopt;
  }

  [[nodiscard]] constexpr std::optional<Color_RGB> parse_color(std::string_view string) noexcept
  {
    if(auto result = parse_color_hex(string); result)
      return result;
    return parse_color_name(string);
  }

  [[nodiscard]] constexpr std::optional<int> parse_small_positive_int(std::string_view string) noexcept
  {
    int result = 0;
    for(char c : string) {
      if(c < '0' || c > '9') {
        return std::nullopt;
      } else {
        result = result * 10 + c - '0';

        if(result > 1000)
          return std::nullopt;
      }
    }

    return result;
  }

  [[nodiscard]] constexpr Options parse_args(std::span<char const* const> args)
  {
    Parser parser = Parser(args);
    Options result {};

    enum class OptionKind
    {
      NONE,
      HELP,
      INPUT_FILE,
      STYLE,
      TREE_ALIGN,
      TEXT_ALIGN,
      TEXT_COLOR,
      BOX_COLOR,
      BRANCH_COLOR,
      LINE_MARGIN,
      SIBLING_MARGIN,
      HORIZONTAL_PADDING,
      VERTICAL_PADDING
    };

    auto const get_option_kind = [](std::string_view name) -> OptionKind {
      using enum OptionKind;
      if(name == "h" || name == "help")
        return HELP;
      if(name == "i" || name == "input" || name == "input-file")
        return INPUT_FILE;
      if(name == "style")
        return STYLE;
      if(name == "tree-align")
        return TREE_ALIGN;
      if(name == "text-align")
        return TEXT_ALIGN;
      if(name == "text-color")
        return TEXT_COLOR;
      if(name == "box-color")
        return BOX_COLOR;
      if(name == "branch-color")
        return BRANCH_COLOR;
      if(name == "line-margin")
        return LINE_MARGIN;
      if(name == "sibling-margin")
        return SIBLING_MARGIN;
      if(name == "horizontal-padding")
        return HORIZONTAL_PADDING;
      if(name == "vertical-padding")
        return VERTICAL_PADDING;
      return NONE;
    };

    using namespace std::string_view_literals;
    using namespace std::string_literals;

    while(!parser.done()) {
      if(Option option = parser.next(); option) {
        if(option.name == "") {
          // positional argument
          result.positional_input = option.value;
          continue;
        }

        // otherwise it's a named option
        switch(get_option_kind(option.name)) {
          case OptionKind::NONE: {
            result.errors.push_back("Unrecognized option '"s + std::string(option.name) + "'");
            break;
          }
          case OptionKind::HELP: {
            result.print_help = true;
            break;
          }
          case OptionKind::INPUT_FILE: {
            if(option.value == "") {
              std::string message = "Invalid usage of --input-file. Expected a file name.";
              result.errors.push_back(std::move(message));
            } else {
              result.input_file_name = option.value;
            }
            break;
          }
          case OptionKind::STYLE: {
            if(option.value == "") {
              std::string message = "Invalid usage of --style. Expected a style name.";
              result.errors.push_back(std::move(message));
            } else if(auto maybe_style = parse_style(option.value); maybe_style) {
              result.style = *maybe_style;
            } else {
              std::string message = "Invalid usage of --style. Unrecognized style '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::TREE_ALIGN: {
            if(option.value == "") {
              std::string message = "Invalid usage of --tree-align. Expected left|center|right.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<Tree_Alignment> maybe_align = parse_tree_align(option.value); maybe_align) {
              result.tree_align = *maybe_align;
            } else {
              std::string message = "Invalid usage of --tree-align. Unrecognized alignment '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::TEXT_ALIGN: {
            if(option.value == "") {
              std::string message = "Invalid usage of --text-align. Expected left|center|right.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<Text_Alignment> maybe_align = parse_text_align(option.value); maybe_align) {
              result.text_align = *maybe_align;
            } else {
              std::string message = "Invalid usage of --text-align. Unrecognized alignment '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::TEXT_COLOR: {
            if(option.value == "") {
              std::string message = "Invalid usage of --text-color. Expected a color name or hex value.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<Color_RGB> maybe_color = parse_color(option.value); maybe_color) {
              result.text_color = *maybe_color;
            } else {
              std::string message = "Invalid usage of --text-color. Unrecognized color '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::BOX_COLOR: {
            if(option.value == "") {
              std::string message = "Invalid usage of --box-color. Expected a color name or hex value.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<Color_RGB> maybe_color = parse_color(option.value); maybe_color) {
              result.box_color = *maybe_color;
            } else {
              std::string message = "Invalid usage of --box-color. Unrecognized color '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::BRANCH_COLOR: {
            if(option.value == "") {
              std::string message = "Invalid usage of --branch-color. Expected a color name or hex value.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<Color_RGB> maybe_color = parse_color(option.value); maybe_color) {
              result.branch_color = *maybe_color;
            } else {
              std::string message = "Invalid usage of --branch-color. Unrecognized color '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::LINE_MARGIN: {
            if(option.value == "") {
              std::string message = "Invalid usage of --line-margin. Expected a positive integer < 1000.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<int> maybe_int = parse_small_positive_int(option.value); maybe_int) {
              result.level_margin = *maybe_int;
            } else {
              std::string message = "Invalid usage of --line-margin. Not valid: '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::SIBLING_MARGIN: {
            if(option.value == "") {
              std::string message = "Invalid usage of --sibling-margin. Expected a positive integer < 1000.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<int> maybe_int = parse_small_positive_int(option.value); maybe_int) {
              result.sibling_margin = *maybe_int;
            } else {
              std::string message = "Invalid usage of --sibling-margin. Not valid: '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::HORIZONTAL_PADDING: {
            if(option.value == "") {
              std::string message = "Invalid usage of --horizontal-padding. Expected a positive integer < 1000.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<int> maybe_int = parse_small_positive_int(option.value); maybe_int) {
              result.horizontal_padding = *maybe_int;
            } else {
              std::string message = "Invalid usage of --horizontal-padding. Not valid: '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
          case OptionKind::VERTICAL_PADDING: {
            if(option.value == "") {
              std::string message = "Invalid usage of --vertical-padding. Expected a positive integer < 1000.";
              result.errors.push_back(std::move(message));
            } else if(std::optional<int> maybe_int = parse_small_positive_int(option.value); maybe_int) {
              result.vertical_padding = *maybe_int;
            } else {
              std::string message = "Invalid usage of --vertical-padding. Not valid: '"s + std::string(option.value) + "'.";
              result.errors.push_back(std::move(message));
            }
            break;
          }
        }
      }
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
