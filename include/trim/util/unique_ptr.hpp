#pragma once
#include <memory>

namespace trim
{
  /*!
   * Usable in constant expression.
   */
  template<typename T>
  struct Unique_Ptr
  {
    private:

    T* m_ptr {};

    public:

    using value_type = T;
    using pointer = T*;
    using reference = T&;

    Unique_Ptr() = default;

    template<std::derived_from<T> U>
    explicit constexpr Unique_Ptr(U* ptr) noexcept
      : m_ptr(ptr)
    {}

    template<std::derived_from<T> U>
    explicit constexpr Unique_Ptr(Unique_Ptr<U>&& other) noexcept
      : m_ptr(other.release())
    {}

    constexpr Unique_Ptr(Unique_Ptr&& other) noexcept
    {
      m_ptr = other.m_ptr;
      other.m_ptr = nullptr;
    }

    constexpr Unique_Ptr& operator=(Unique_Ptr&& other) noexcept
    {
      delete m_ptr;
      m_ptr = other.m_ptr;
      other.m_ptr = nullptr;
      return *this;
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
      return m_ptr != nullptr;
    }

    [[nodiscard]] constexpr pointer get() const noexcept
    {
      return m_ptr;
    }

    [[nodiscard]] constexpr pointer operator->() const noexcept
    {
      return m_ptr;
    }

    [[nodiscard]] constexpr reference operator*() const noexcept
    {
      return *m_ptr;
    }

    [[nodiscard]] constexpr pointer release() noexcept
    {
      pointer result = m_ptr;
      m_ptr = nullptr;
      return result;
    }

    constexpr void reset() noexcept
    {
      delete m_ptr;
      m_ptr = nullptr;
    }

    constexpr ~Unique_Ptr() noexcept
    {
      delete m_ptr;
    }

    friend constexpr void swap(Unique_Ptr& lhs, Unique_Ptr& rhs) noexcept
    {
      std::swap(lhs.m_ptr, rhs.m_ptr);
    }

    bool operator==(Unique_Ptr const&) const = default;
    auto operator<=>(Unique_Ptr const&) const = default;
  };

  template<typename T, typename... Args>
  [[nodiscard]] constexpr Unique_Ptr<T> make_unique(Args&&... args) //
    noexcept(false)                                                 //
    requires(std::is_constructible_v<T, Args...>)
  {
    return Unique_Ptr<T>(new T(std::forward<Args>(args)...));
  }
} // namespace trim
