#pragma once
#include <trim/util/assert.hpp>

#include <optional>
#include <string_view>

namespace trim::cli
{
  template<typename T>
  concept Is_Option = requires(T const& option) {
    // clang-format off
    { option.name() } -> std::same_as<std::optional<std::string_view>>;
    { option.value() } -> std::same_as<std::optional<std::string_view>>;
    // clang-format on
  };

  struct Flag_Option
  {
    std::string_view m_name {};

    Flag_Option() = default;

    explicit constexpr Flag_Option(std::string_view name) noexcept
      : m_name(name)
    {}

    [[nodiscard]] constexpr std::optional<std::string_view> name() const noexcept
    {
      return m_name;
    }

    [[nodiscard]] constexpr std::optional<std::string_view> value() const noexcept
    {
      return std::nullopt;
    }

    bool operator==(Flag_Option const&) const = default;
    auto operator<=>(Flag_Option const&) const = default;
  };

  struct Value_Option
  {
    std::string_view m_name {};
    std::string_view m_value {};

    Value_Option() = default;

    explicit constexpr Value_Option(std::string_view name, std::string_view value) noexcept
      : m_name(name)
      , m_value(value)
    {}

    [[nodiscard]] constexpr std::optional<std::string_view> name() const noexcept
    {
      return m_name;
    }

    [[nodiscard]] constexpr std::optional<std::string_view> value() const noexcept
    {
      return m_value;
    }

    bool operator==(Value_Option const&) const = default;
    auto operator<=>(Value_Option const&) const = default;
  };

  struct Positional_Option
  {
    std::string_view m_value {};

    Positional_Option() = default;

    explicit constexpr Positional_Option(std::string_view value) noexcept
      : m_value(value)
    {}

    [[nodiscard]] constexpr std::optional<std::string_view> name() const noexcept
    {
      return std::nullopt;
    }

    [[nodiscard]] constexpr std::optional<std::string_view> value() const noexcept
    {
      return m_value;
    }

    bool operator==(Positional_Option const&) const = default;
    auto operator<=>(Positional_Option const&) const = default;
  };

  struct Option
  {
    private:

    enum class Kind
    {
      EMPTY,
      FLAG,
      VALUE,
      POSITIONAL,
    };

    struct Empty
    {};

    union Union
    {
      Empty m_empty {};
      Flag_Option m_flag;
      Value_Option m_value;
      Positional_Option m_positional;
    };

    Union m_value {};
    Kind m_kind {};

    public:

    Option() = default;

    constexpr Option(Flag_Option flag) noexcept
      : m_value({.m_flag = flag})
      , m_kind(Kind::FLAG)
    {}

    constexpr Option(Value_Option value) noexcept
      : m_value({.m_value = value})
      , m_kind(Kind::VALUE)
    {}

    constexpr Option(Positional_Option positional) noexcept
      : m_value({.m_positional = positional})
      , m_kind(Kind::POSITIONAL)
    {}

    [[nodiscard]] constexpr std::optional<std::string_view> name() const
    {
      switch(m_kind) {
        case Kind::EMPTY: throw std::runtime_error("access empty option");
        case Kind::FLAG: return m_value.m_flag.name();
        case Kind::VALUE: return m_value.m_value.name();
        case Kind::POSITIONAL: return m_value.m_positional.name();
      }

      TRIM_ASSERT(false);
    }

    [[nodiscard]] constexpr std::optional<std::string_view> value() const
    {
      switch(m_kind) {
        case Kind::EMPTY: throw std::runtime_error("access empty option");
        case Kind::FLAG: return m_value.m_flag.value();
        case Kind::VALUE: return m_value.m_value.value();
        case Kind::POSITIONAL: return m_value.m_positional.value();
      }

      TRIM_ASSERT(false);
    }

    [[nodiscard]] constexpr bool operator==(Option const& other) const noexcept
    {
      if(m_kind != other.m_kind)
        return false;

      switch(m_kind) {
        case Kind::EMPTY: return true;
        case Kind::FLAG: return m_value.m_flag == other.m_value.m_flag;
        case Kind::VALUE: return m_value.m_value == other.m_value.m_value;
        case Kind::POSITIONAL: return m_value.m_positional == other.m_value.m_positional;
      }

      TRIM_ASSERT(false);
    }
  };

  static_assert(Is_Option<Flag_Option>);
  static_assert(Is_Option<Value_Option>);
  static_assert(Is_Option<Positional_Option>);
  static_assert(Is_Option<Option>);

} // namespace trim::cli
