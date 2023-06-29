#pragma once
#include <trim/util/assert.hpp>

#include <cstdint>
#include <span>

namespace trim
{
  /*!
   * Implements operator<< but writes to a constant size buffer.
   * Usable in constant expressions.
   */
  struct Memory_OStream
  {
    using size_type = std::size_t;
    using char_type = char;
    using pointer = char_type*;

    pointer m_buffer {};
    pointer m_pos {};
    size_type m_buffer_size {};

    Memory_OStream() = default;

    explicit constexpr Memory_OStream(pointer buffer, size_type size) noexcept
      : m_buffer(buffer)
      , m_pos(buffer)
      , m_buffer_size(size)
    {}

    explicit constexpr Memory_OStream(std::span<char_type> buffer) noexcept
      : m_buffer(buffer.data())
      , m_pos(buffer.data())
      , m_buffer_size(buffer.size())
    {}

    [[nodiscard]] constexpr size_type remaining_capacity() const noexcept
    {
      return m_buffer_size - (m_pos - m_buffer);
    }

    friend constexpr Memory_OStream& operator<<(Memory_OStream& stream, std::string_view string) noexcept
    {
      TRIM_ASSERT(stream.remaining_capacity() >= string.size());

      for(char c : string) {
        *stream.m_pos = c;
        ++stream.m_pos;
      }

      return stream;
    }

    template<std::same_as<char> T>
    friend constexpr Memory_OStream& operator<<(Memory_OStream& stream, T character) noexcept
    {
      TRIM_ASSERT(stream.remaining_capacity() > 0);

      *stream.m_pos = character;
      ++stream.m_pos;
      return stream;
    }
  };
} // namespace trim
