#pragma once
#include <trim/util/assert.hpp>

#include <string_view>
#include <vector>

namespace trim
{
  /*!
   * Usable in constant expression.
   */
  struct String
  {
    public:

    using value_type = char;
    using reference = char&;
    using const_reference = char const&;
    using pointer = char*;
    using const_pointer = char const*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using size_type = std::size_t;
    using ssize_type = std::make_signed_t<std::size_t>;

    private:

    std::vector<value_type> m_string {};

    public:

    String() = default;

    explicit constexpr String(std::string_view init)
      : m_string(init.data(), init.data() + init.size())
    {}

    explicit constexpr String(char const* init, std::size_t len)
      : m_string(init, init + len)
    {}

    [[nodiscard]] explicit constexpr operator std::string_view() const noexcept
    {
      return std::string_view(m_string.data(), m_string.size());
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
      return m_string.size();
    }

    [[nodiscard]] constexpr ssize_type ssize() const noexcept
    {
      return static_cast<ssize_type>(m_string.size());
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
      return m_string.size() == 0;
    }

    [[nodiscard]] constexpr reference operator[](size_type index) & noexcept
    {
      TRIM_ASSERT(index < size());
      return m_string[index];
    }

    [[nodiscard]] constexpr const_reference operator[](size_type index) const& noexcept
    {
      TRIM_ASSERT(index < size());
      return m_string[index];
    }

    constexpr void append(std::string_view other) &
    {
      m_string.insert(m_string.end(), other.data(), other.data() + other.size());
    }

    constexpr void append(char character) &
    {
      m_string.push_back(character);
    }

    template<typename Stream>
    [[nodiscard]] friend constexpr Stream& operator<<(Stream& stream, String const& string)
    {
      return stream << std::string_view(string);
    }

    [[nodiscard]] constexpr bool operator==(String const& other) const = default;
    [[nodiscard]] constexpr auto operator<=>(String const& other) const = default;
  };
} // namespace trim
