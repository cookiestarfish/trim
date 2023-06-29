#pragma once
#include <trim/parsing/parser.hpp>
#include <trim/util/assert.hpp>
#include <trim/util/format_int.hpp>
#include <trim/util/split.hpp>

#include <algorithm>
#include <vector>

namespace trim
{
  struct Parentheses_Parser
  {
    [[nodiscard]] static constexpr Parse_Result parse(std::string_view text)
    {
      // for each opening paren, compute the index of the matching closing paren
      auto const compute_matching_closed_paren = [](std::string_view string) constexpr -> std::vector<size_type> {
        std::vector<size_type> opening_index {};
        std::vector<size_type> closing_index = std::vector<size_type>(string.size(), size_type(-1));
        for(size_type i = 0; i < string.size(); ++i) {
          if(string[i] == '(') {
            opening_index.push_back(i);
          } else if(string[i] == ')') {
            if(!opening_index.empty()) {
              closing_index[opening_index.back()] = i;
              opening_index.pop_back();
            }
          }
        }
        return closing_index;
      };

      Parse_Result result {};
      std::vector<size_type> const closing_paren = compute_matching_closed_paren(text);
      size_type const first_paren = text.find('(');

      for(size_type i = 0; i < text.size(); ++i) {
        if(text[i] == '(' && closing_paren[i] == size_type(-1)) {
          std::string message = "Unbalanced parentheses.";
          result.errors.push_back(Parse_Error(std::move(message), i, 1));
        }
      }

      size_type const last_paren = closing_paren[first_paren];
      size_type const num_nodes = std::ranges::count(text.substr(0, last_paren + 1), '(');
      size_type const num_parens = std::ranges::count(text, '(');

      if(!result.errors.empty())
        return result;

      if(num_parens != num_nodes) {
        result.errors.push_back(Parse_Error("The tree is not connected", last_paren + 1, 1));
      }

      Labels node_labels = Labels(num_nodes);
      Labels edge_labels = Labels(num_nodes);
      Tree tree = Tree(num_nodes);

      size_type free_id = 0;
      auto const recurse = [&](auto& rec, size_type begin, size_type end) -> size_type {
        size_type curr_id = free_id;
        free_id += 1;
        trim::String label {};

        while(begin != end) {
          if(text.substr(begin).starts_with("\\n")) {
            begin += 2;
            label.append("\n");
            continue;
          }

          if(text.substr(begin).starts_with('(')) {
            size_type child_id = rec(rec, begin + 1, closing_paren[begin]);
            begin = closing_paren[begin] + 1;
            tree.add_child(curr_id, child_id);
            continue;
          }

          label.append(text[begin]);
          begin += 1;
        }

        if(label.empty())
          label = trim::format_integer_to_string(curr_id);

        node_labels[curr_id] = std::move(label);
        edge_labels[curr_id] = trim::String();
        return curr_id;
      };
      size_type root = recurse(recurse, first_paren + 1, closing_paren[first_paren]);
      return Parse_Result(std::move(tree), root, std::move(node_labels), std::move(edge_labels));
    }
  };
} // namespace trim
