#pragma once
#include <trim/parsing/parser.hpp>
#include <trim/util/assert.hpp>
#include <trim/util/format_int.hpp>
#include <trim/util/split.hpp>

#include <algorithm>

namespace trim
{
  struct Markdown_Parser
  {
    [[nodiscard]] static constexpr Parse_Result parse(std::string_view text)
    {
      std::vector<std::string_view> lines {};
      std::vector<size_type> levels {};

      trim::split_string_by_newline(text, [&](std::string_view line) {
        if(line.starts_with("#")) {
          lines.push_back(line);
          levels.push_back(std::ranges::count(line, '#'));
        }
      });

      size_type const num_nodes = lines.size();
      Tree tree = Tree(num_nodes);
      Labels node_labels = Labels(num_nodes);
      Labels edge_labels = Labels(num_nodes);

      auto const recurse = [&](auto& rec, size_type curr) -> size_type {
        TRIM_ASSERT(curr < lines.size());

        size_type child = curr + 1;
        size_type count = 1;

        while(child < num_nodes && levels[child] > levels[curr]) {
          size_type child_lines = rec(rec, child);
          tree.add_child(curr, child);
          child += child_lines;
          count += child_lines;
        }

        return count;
      };
      recurse(recurse, 0);

      for(size_type i = 0; i < num_nodes; ++i) {
        std::string_view label = lines[i];
        while(label.starts_with('#'))
          label.remove_prefix(1);
        node_labels[i] = trim::String(label);

        if(label.empty())
          node_labels[i] = trim::format_integer_to_string(i);
      }

      Parse_Result result {};
      result.tree = std::move(tree);
      result.node_labels = std::move(node_labels);
      result.edge_labels = std::move(edge_labels);
      result.root = 0;
      return result;
    }
  };
} // namespace trim
