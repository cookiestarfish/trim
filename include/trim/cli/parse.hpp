#pragma once
#include <trim/cli/option.hpp>

#include <array>
#include <span>

namespace trim::cli
{
  struct Parse_Result
  {
    std::array<Option, 64> m_parsed_options {};
    std::size_t m_num_parsed_options {};

    std::array<std::string_view, 8> m_unparsed_options {};
    std::size_t m_num_unparsed_options {};

    [[nodiscard]] constexpr std::span<Option const> parsed_arguments() const noexcept
    {
      return std::span<Option const>(m_parsed_options.data(), m_num_parsed_options);
    }

    [[nodiscard]] constexpr std::span<std::string_view const> unparsed_arguments() const noexcept
    {
      return std::span<std::string_view const>(m_unparsed_options.data(), m_num_unparsed_options);
    }
  };

  [[nodiscard]] constexpr bool is_alpha(char x) noexcept
  {
    return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z');
  }

  [[nodiscard]] constexpr bool is_digit(char x) noexcept
  {
    return (x >= '0' && x <= '9');
  }

  [[nodiscard]] constexpr bool is_ascii(char x) noexcept
  {
    return static_cast<unsigned>(x) <= 127;
  }

  [[nodiscard]] constexpr std::string_view parse_xid_start(std::string_view string) noexcept
  {
    if(string.empty())
      return string.substr(0, 0);

    if(is_ascii(string[0])) {
      char front = string[0];
      if(is_alpha(front) || front == '_') {
        return string.substr(0, 1);
      } else {
        return string.substr(0, 0);
      }
    }

    std::size_t length = 0;
    while(length < string.length() && !is_ascii(string[length]))
      length += 1;
    return string.substr(0, length);
  }

  [[nodiscard]] constexpr std::string_view parse_xid_continue(std::string_view string) noexcept
  {
    if(string.empty())
      return string.substr(0, 0);

    if(is_ascii(string[0])) {
      char front = string[0];
      if(is_alpha(front) || is_digit(front) || front == '_' || front == '-')
        return string.substr(0, 1);
      return string.substr(0, 0);
    }

    std::size_t length = 0;
    while(length < string.length() && !is_ascii(string[length]))
      length += 1;
    return string.substr(0, length);
  }

  [[nodiscard]] constexpr std::string_view parse_identifier(std::string_view string) noexcept
  {
    if(string.empty())
      return string.substr(0, 0);

    std::string_view backup = string;
    std::string_view curr = parse_xid_start(string);
    std::size_t length = curr.length();
    while(!curr.empty()) {
      string.remove_prefix(curr.length());
      curr = parse_xid_continue(string);
      length += curr.length();
    }

    return backup.substr(0, length);
  }

  [[nodiscard]] constexpr Parse_Result parse(std::span<char const* const> args) noexcept
  {
    Parse_Result result {};
    constexpr std::size_t max_unparsed_options = 8;
    constexpr std::size_t max_parsed_options = 64;

    auto const add_unparsed_option = [&](std::string_view option) -> void {
      if(result.m_num_unparsed_options == max_unparsed_options)
        return;
      result.m_unparsed_options[result.m_num_unparsed_options] = option;
      result.m_num_unparsed_options += 1;
    };

    auto const add_parsed_option = [&](Option option) -> void {
      if(result.m_num_parsed_options == max_parsed_options)
        return;
      result.m_parsed_options[result.m_num_parsed_options] = std::move(option);
      result.m_num_parsed_options += 1;
    };

    bool positional_only = false;

    while(!args.empty()) {
      std::string_view option = args[0];
      std::string_view const backup = option;
      bool is_flag = 0;
      bool is_value = 0;
      bool is_positional = positional_only;

      std::string_view name = "";
      std::string_view value = "";

      if(is_positional) {
        value = option;
        option = "";
      }

      if(option == "--") {
        positional_only = true;
        args = args.subspan(1);
        continue;
      }

      if(option.starts_with("--")) {
        is_value = 1;
        option.remove_prefix(2);
      } else if(option.starts_with("-")) {
        is_flag = 1;
        option.remove_prefix(1);
      } else {
        is_positional = 1;
        value = option;
        option = "";
      }

      name = parse_identifier(option);
      option.remove_prefix(name.length());

      if(option.starts_with("=")) {
        option.remove_prefix(1);
        value = option;
        option = "";
      }

      bool is_option_fully_consumed = (option.empty());
      bool is_name_empty = (name == "");
      bool is_flag_usage_correct = (is_flag && value == "") || (!is_flag);

      if(!is_option_fully_consumed) {
        add_unparsed_option(backup);
        args = args.subspan(1);
        continue;
      }

      if(is_positional) {
        add_parsed_option(Positional_Option(value));
        args = args.subspan(1);
        continue;
      }

      if(is_name_empty) {
        add_unparsed_option(backup);
        args = args.subspan(1);
        continue;
      }

      if(!is_flag_usage_correct) {
        add_unparsed_option(backup);
        args = args.subspan(1);
        continue;
      }

      if(is_flag) {
        add_parsed_option(Flag_Option(name));
        args = args.subspan(1);
        continue;
      }

      if(is_value && value != "") {
        add_parsed_option(Value_Option(name, value));
        args = args.subspan(1);
        continue;
      }

      if(args.size() > 1) {
        value = args[1];
        if(!value.starts_with("-")) {
          add_parsed_option(Value_Option(name, value));
          args = args.subspan(2);
          continue;
        }
      }

      add_parsed_option(Flag_Option(name));
      args = args.subspan(1);
    }

    return result;
  }
} // namespace trim::cli
