#pragma once
#include "trim/color/rgb.hpp"
#include <trim/style/style.hpp>
#include <trim/util/geometry.hpp>
#include <trim/util/ints.hpp>
#include <trim/util/unique_ptr.hpp>

#include <string_view>

namespace trim
{
  // clang-format off

  enum class Sprite_Category
  {
    NONE    = 0b0000,
    NODE    = 0b0001,
    BRANCH  = 0b0010,
    TEXT    = 0b0100,
  };

  // clang-format on

  [[nodiscard]] constexpr bool operator&(Sprite_Category lhs, Sprite_Category rhs) noexcept
  {
    return static_cast<bool>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
  }

  [[nodiscard]] constexpr Sprite_Category operator|(Sprite_Category lhs, Sprite_Category rhs) noexcept
  {
    return Sprite_Category(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
  }

  struct Draw_Result
  {
    std::string_view character {};
    Color_RGB color {};

    Draw_Result() = default;

    explicit constexpr Draw_Result(std::string_view character, Color_RGB color) noexcept
      : character(character)
      , color(color)
    {}

    bool operator==(Draw_Result const&) const = default;
    auto operator<=>(Draw_Result const&) const = default;
  };

  template<typename T>
  concept Is_Sprite = requires(T const& sprite, Point cursor, Style const& style) {
    // clang-format off
    { sprite.rect() } noexcept -> std::same_as<Rect>;
    { sprite.draw(style, cursor) } noexcept -> std::same_as<Draw_Result>;
    { sprite.category() } noexcept -> std::same_as<Sprite_Category>;
    // clang-format on
  };

  struct Sprite
  {
    private:

    struct Interface
    {
      [[nodiscard]] constexpr virtual Rect rect() const noexcept = 0;
      [[nodiscard]] constexpr virtual Draw_Result draw(Style const& style, Point cursor) const noexcept = 0;
      [[nodiscard]] constexpr virtual Sprite_Category category() const noexcept = 0;

      virtual constexpr ~Interface() noexcept
      {}
    };

    template<Is_Sprite T>
    struct Storage final : Interface
    {
      T m_value {};

      Storage() = default;

      constexpr Storage(T const& init)                    //
        noexcept(std::is_nothrow_copy_constructible_v<T>) //
        requires(std::is_copy_constructible_v<T>)         //
        : m_value(init)
      {}

      constexpr Storage(T&& init)                         //
        noexcept(std::is_nothrow_move_constructible_v<T>) //
        requires(std::is_move_constructible_v<T>)         //
        : m_value(std::move(init))
      {}

      [[nodiscard]] constexpr Rect rect() const noexcept override
      {
        return m_value.rect();
      }

      [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept override
      {
        Draw_Result result = m_value.draw(style, cursor);

        Sprite_Category const cat = m_value.category();

        auto const pick_color = [&]() -> Color_RGB {
          std::size_t seed = 42;
          seed ^= trim::splitmix64(static_cast<unsigned>(cat));
          seed ^= trim::splitmix64(static_cast<std::uint_least64_t>(cursor.line));
          seed ^= trim::splitmix64(static_cast<std::uint_least64_t>(cursor.column));
          if(!std::is_constant_evaluated()) {
            seed ^= trim::splitmix64(std::uintptr_t(&m_value));
          }
          return pick_rainbow(seed);
        };

        if(cat == Sprite_Category::BRANCH && style.branch_color == Color_RGB::RAINBOW) {
          result.color = pick_color();
        }

        if(cat == Sprite_Category::NODE && style.box_color == Color_RGB::RAINBOW) {
          result.color = pick_color();
        }

        if(cat == Sprite_Category::TEXT && style.text_color == Color_RGB::RAINBOW) {
          result.color = pick_color();
        }

        return result;
      }

      [[nodiscard]] constexpr Sprite_Category category() const noexcept override
      {
        return m_value.category();
      }
    };

    Unique_Ptr<Interface> m_ptr {};

    public:

    Sprite() = default;

    template<typename T>
      requires(!std::is_same_v<T, Sprite> && Is_Sprite<T>)
    constexpr Sprite(T init)                            //
      noexcept(std::is_nothrow_move_constructible_v<T>) //
      requires(std::is_move_constructible_v<T>)         //
      : m_ptr(make_unique<Storage<T>>(std::move(init)))
    {}

    [[nodiscard]] constexpr Rect rect() const noexcept
    {
      return m_ptr->rect();
    }

    [[nodiscard]] constexpr Draw_Result draw(Style const& style, Point cursor) const noexcept
    {
      return m_ptr->draw(style, cursor);
    }

    [[nodiscard]] constexpr Sprite_Category category() const noexcept
    {
      return m_ptr->category();
    }
  };
} // namespace trim
