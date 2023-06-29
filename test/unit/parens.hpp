#pragma once
#include <trim/trim.hpp>
#include <trim/util/memory_ostream.hpp>

namespace trim::detail::test
{

  static consteval auto compute_result(std::string_view input) noexcept
  {
    auto parser = trim::Parentheses_Parser {};
    auto parsed = parser.parse(input);
    TRIM_ASSERT(parsed.errors.empty());
    auto layout = make_layout(parsed.tree, parsed.root, parsed.node_labels, parsed.edge_labels, default_style);
    TRIM_ASSERT(layout.size() == parsed.tree.size());
    auto tree = Tree_Sprite(parsed.tree, parsed.root, parsed.node_labels, parsed.edge_labels, layout);
    auto scene = Scene(std::move(tree));

    std::array<char, 4096> buffer {};
    Memory_OStream ostream = Memory_OStream(buffer);
    scene.draw(ostream, default_style);
    return buffer;
  }

  template<std::array data = compute_result("(()())")>
  static constexpr bool test1() noexcept
  {
    using namespace std::string_view_literals;
    constexpr std::string_view view = std::string_view(data.data());
    constexpr std::string_view expected = R"EOF(
   ┌───┐    
   | 0 |    
   └─┬─┘    
  ┌──┴───┐  
┌─┴─┐  ┌─┴─┐
| 1 |  | 2 |
└───┘  └───┘
)EOF"sv.substr(1);

    static_assert(view == expected);
    return true;
  }

  template<std::array data = compute_result("(()()(()))")>
  static constexpr bool test2() noexcept
  {
    using namespace std::string_view_literals;
    constexpr std::string_view view = std::string_view(data.data());
    constexpr std::string_view expected = R"EOF(
       ┌───┐       
       | 0 |       
       └─┬─┘       
  ┌──────┼──────┐  
┌─┴─┐  ┌─┴─┐  ┌─┴─┐
| 1 |  | 2 |  | 3 |
└───┘  └───┘  └─┬─┘
                |  
              ┌─┴─┐
              | 4 |
              └───┘
)EOF"sv.substr(1);

    static_assert(view == expected);
    return true;
  }

  static_assert(test1());
  static_assert(test2());

} // namespace trim::detail::test
