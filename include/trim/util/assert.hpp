#pragma once
#include <trim/port/compiler.hpp>

#include <cstdio>
#include <stdexcept>

#define TRIM_ASSERT(...)                                                                                                                                       \
  do {                                                                                                                                                         \
    if(__VA_ARGS__) {                                                                                                                                          \
    } else {                                                                                                                                                   \
      ::trim::report_assert_failure({__FILE__, __LINE__, __func__, #__VA_ARGS__});                                                                             \
    }                                                                                                                                                          \
  } while(false)

namespace trim
{
  struct Assert_Failure
  {
    char const* file_name;
    int line;
    char const* function_name;
    char const* message;

    consteval Assert_Failure(char const* file_name, int line, char const* function_name, char const* message) noexcept
      : file_name(file_name)
      , line(line)
      , function_name(function_name)
      , message(message)
    {}
  };

  [[noreturn]] constexpr void report_assert_failure(Assert_Failure info, bool always_true = true) noexcept
  {
    // avoids 'x function never produces a constant expression' error on some compilers.
    if(always_true) {
      if(std::is_constant_evaluated()) {
        TRIM_CRASH();
      }

      std::fprintf(stderr, "%s:%d internal check failed in %s: %s\n", info.file_name, info.line, info.function_name, info.message);
      std::fflush(stderr);
      TRIM_CRASH();
    }

    TRIM_UNREACHABLE();
  }
} // namespace trim
