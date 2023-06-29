#include <trim/cli/cli.hpp>
#include <trim/color/rgb.hpp>
#include <trim/layout/make_layout.hpp>
#include <trim/layout/tree_layout.hpp>
#include <trim/parsing/bitstring.hpp>
#include <trim/parsing/markdown.hpp>
#include <trim/parsing/parentheses.hpp>
#include <trim/scene/scene.hpp>
#include <trim/style/style.hpp>

#include <fstream>
#include <iostream>
#include <string>

template<typename Stream>
[[nodiscard]] constexpr std::string read_input(Stream& stream)
{
  std::string result {};
  std::string line {};
  while(std::getline(stream, line)) {
    result.append(line);
    result.append("\n");
  }
  return result;
}

int main(int argc, char** argv)
{
  using arg_span = std::span<char const* const>;
  arg_span args = arg_span(argv + 1, argc - 1);

  // parse command line arguments
  trim::cli::Options cli = trim::cli::parse_arguments(args);

  // print help
  if(cli.print_help) {
    std::cout << trim::cli::help_message() << std::endl;
    return 0;
  }

  // print command line argument errors
  if(!cli.errors.empty()) {
    for(auto& error : cli.errors) {
      std::cerr << error << std::endl;
    }
    return 1;
  }

  // read input from stdin, file or command line positional arguments

  std::string input_text {};

  if(cli.input_file_name) {
    // read from file

    std::string file_name = std::string(cli.input_file_name.value());
    std::ifstream input_file = std::ifstream(std::string(file_name));
    if(!input_file.is_open()) {
      std::cerr << "Could not open file " << file_name << std::endl;
      return 1;
    }

    input_text = read_input(input_file);
  } else if(cli.positional_input) {
    // read from positional arguments

    input_text = std::string(cli.positional_input.value());
  } else {
    // read stdin

    input_text = read_input(std::cin);
  }

  // parse the input
  trim::Bitstring_Parser bitstring_parser {};
  trim::Markdown_Parser markdown_parser {};
  trim::Parentheses_Parser parentheses_parser {};
  trim::Parse_Result parsed {};

  // choose the parser to use based on heuristics
  if(std::ranges::count(input_text, '#') > 0) {
    // use the markdown parser
    parsed = markdown_parser.parse(input_text);
  } else if(!input_text.empty() && (input_text[0] == '0' || input_text[0] == '1')) {
    // use the bitstring parser
    parsed = bitstring_parser.parse(input_text);
  } else if(!input_text.empty() && (input_text[0] == '(')) {
    // use the parentheses parser
    parsed = parentheses_parser.parse(input_text);
  } else {
    std::cerr << "Can't parse the given input.\n";
    std::cerr << "Allowed inputs are markdown, balanced parentheses, or a string of binary digits.\n";
    return 1;
  }

  // print parser errors
  // TODO: report error line numbers
  if(!parsed.errors.empty()) {
    for(auto& error : parsed.errors) {
      std::cerr << error.message << " (pos: " << error.position << ")" << std::endl;
    }
    return 1;
  }

  // configure the global style
  // TODO: allow different style for each node
  trim::Style style = trim::default_style;

  if(cli.style) {
    style = cli.style.value();
  }

  if(cli.text_modifier)
    style.text_modifier = cli.text_modifier.value();
  if(cli.text_align)
    style.text_align = cli.text_align.value();
  if(cli.horizontal_padding)
    style.node_horizontal_padding = cli.horizontal_padding.value();
  if(cli.vertical_padding)
    style.node_vertical_padding = cli.vertical_padding.value();
  if(cli.sibling_margin)
    style.sibling_margin = cli.sibling_margin.value();
  if(cli.level_margin)
    style.level_margin = cli.level_margin.value();
  if(cli.node_min_width)
    style.node_minimum_width = cli.node_min_width.value();
  if(cli.node_min_height)
    style.node_minimum_height = cli.node_min_height.value();
  if(cli.tree_align)
    style.tree_align = cli.tree_align.value();
  if(cli.box_color)
    style.box_color = cli.box_color.value();
  if(cli.text_color)
    style.text_color = cli.text_color.value();
  if(cli.branch_color)
    style.branch_color = cli.branch_color.value();

  trim::Tree_Layout layout = trim::make_layout(parsed.tree, parsed.root, parsed.node_labels, parsed.edge_labels, style);
  trim::Tree_Sprite sprite = trim::Tree_Sprite(parsed.tree, parsed.root, parsed.node_labels, parsed.edge_labels, layout);
  trim::Scene scene = trim::Scene(std::move(sprite));
  scene.draw(std::cout, style);

  return 0;
}
