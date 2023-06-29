#pragma once
#include <stdexcept>

// === compiler detection

#if not defined(TRIM_COMPILER_IS_GCC) and defined(__GNUC__) and not defined(__clang__)
#  define TRIM_COMPILER_IS_GCC 1
#endif

#if not defined(TRIM_COMPILER_IS_CLANG) and defined(__clang__)
#  define TRIM_COMPILER_IS_CLANG 1
#endif

#if not defined(TRIM_COMPILER_IS_MSVC) and defined(_MSC_VER_)
#  define TRIM_COMPILER_IS_MSVC 1
#endif

// === compiler detection (fallback values)

#if not defined(TRIM_COMPILER_IS_GCC)
#  define TRIM_COMPILER_IS_GCC 0
#endif

#if not defined(TRIM_COMPILER_IS_CLANG)
#  define TRIM_COMPILER_IS_CLANG 0
#endif

#if not defined(TRIM_COMPILER_IS_MSVC)
#  define TRIM_COMPILER_IS_MSVC 0
#endif

// === unreachable

#if TRIM_COMPILER_IS_MSVC
#  define TRIM_UNREACHABLE() __builtin_assume(0)
#elif TRIM_COMPILER_IS_GCC
#  define TRIM_UNREACHABLE() __builtin_unreachable()
#elif TRIM_COMPILER_IS_CLANG
#  define TRIM_UNREACHABLE() __builtin_unreachable()
#else
#  define TRIM_UNREACHABLE() __builtin_unreachable()
#endif

// === crash

#if TRIM_COMPILER_IS_MSVC
#  include <intrin.h>
#  define TRIM_CRASH() ::__debugbreak()
#elif TRIM_COMPILER_IS_CLANG
#  define TRIM_CRASH() __builtin_trap()
#elif TRIM_COMPILER_IS_GCC
#  define TRIM_CRASH() __builtin_trap()
#else
#  define TRIM_CRASH() ::std::abort()
#endif
