#pragma once
#include <trim/parsing/parser.hpp>
#include <trim/util/assert.hpp>
#include <trim/util/format_int.hpp>

namespace trim
{
  struct Bitstring_Parser
  {
    [[nodiscard]] static constexpr Parse_Result parse(std::string_view text)
    {
      Parse_Result result {};

      constexpr std::size_t sentinel = std::size_t(-1);
      using bit_type = char8_t;

      if(text.empty()) {
        return result;
      }

      if(text[0] == '0') {
        if(text.size() > 1) {
          result.errors.push_back(Parse_Error("Input not fully consumed (1 character read).", 1, text.size() - 1));
        }

        return result;
      }

      if(text[0] != '1') {
        result.errors.push_back(Parse_Error("Invalid first character, expected '0' or '1'.", 0, 1));
        return result;
      }

      std::vector<std::size_t> parent_array {};
      std::vector<bit_type> touched = std::vector<bit_type>(text.size());

      std::size_t free_id = 0;
      auto const recurse = [&](auto& rec, std::size_t index) noexcept -> std::size_t {
        TRIM_ASSERT(index < text.size());
        TRIM_ASSERT(text[index] == '1');

        std::size_t const left = index * 2 + 1;
        std::size_t const right = index * 2 + 2;
        std::size_t left_id = sentinel;
        std::size_t right_id = sentinel;

        if(left >= text.size()) {
          // (child does not exist)
        } else if(text[left] == '0') {
          // (child does not exist)
          touched[left] = true;
        } else if(text[left] == '1') {
          left_id = rec(rec, left);
        } else {
          // invalid character
          char invalid_character = text[index];
          std::string message = std::string("Invalid character '") + invalid_character + "'";
          result.errors.push_back(Parse_Error(std::move(message), index, 1));
          touched[left] = true;
        }

        parent_array.push_back(sentinel);
        std::size_t const curr_id = free_id;
        free_id += 1;
        touched[index] = true;

        if(right >= text.size()) {
          // do nothing (child does not exist)
        } else if(text[right] == '0') {
          // do nothing (child does not exist)
          touched[right] = true;
        } else if(text[right] == '1') {
          right_id = rec(rec, right);
        } else {
          // invalid character
          char invalid_character = text[index];
          std::string message = std::string("Invalid character '") + invalid_character + "'";
          result.errors.push_back(Parse_Error(std::move(message), index, 1));
          touched[right] = true;
        }

        if(left_id != sentinel) {
          parent_array[left_id] = curr_id;
        }

        if(right_id != sentinel) {
          parent_array[right_id] = curr_id;
        }

        return curr_id;
      };
      std::size_t const root = recurse(recurse, 0);

      std::size_t last_touched = sentinel;
      for(std::size_t i = 0; i < text.size(); ++i)
        if(touched[i])
          last_touched = i;

      for(std::size_t i = 0; i < text.size(); ++i) {
        if(!touched[i] && i > last_touched) {
          std::string message = "Trailing characters not consumed.";
          result.errors.push_back(Parse_Error(std::move(message), i, 1));
        }
      }

      std::size_t const num_nodes = free_id;
      result.tree = Tree(num_nodes);
      result.node_labels = Labels(num_nodes);
      result.edge_labels = Labels(num_nodes);

      for(std::size_t i = 0; i < num_nodes; ++i) {
        std::size_t parent = parent_array[i];
        if(parent != sentinel) {
          result.tree.add_child(parent, i);
        }
        result.node_labels[i] = trim::format_integer_to_string(i);
        result.edge_labels[i] = trim::String();
      }

      result.root = root;
      return result;
    }
  };
} // namespace trim
