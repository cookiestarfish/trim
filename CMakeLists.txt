cmake_minimum_required(VERSION 3.24.0)

## ================================================================================================
## Setup project
## ================================================================================================

project(trim VERSION 0.1.0
  DESCRIPTION "Visualize trees in the terminal" HOMEPAGE_URL "https://www.example.com" LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_CURRENT_BINARY_DIR})

include(GNUInstallDirs)

## ================================================================================================
## No in-source build
## ================================================================================================

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  message(FATAL_ERROR "[trim] Building in-source not recommended! Build in a separate directory.")
endif()

# =================================================================================================
# Options
# =================================================================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(TRIM_ENABLE_TESTS           "Build tests for trim"             OFF)
option(TRIM_ENABLE_DOCS            "Build doxygen documentation"      OFF)

# =================================================================================================
# Library target
# =================================================================================================

add_library(trim INTERFACE)
add_library(trim::trim ALIAS trim)
target_compile_features(trim INTERFACE cxx_std_20)
target_include_directories(trim INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

## =====================================================================================================
## Executable target
## =====================================================================================================

add_executable(trim_exe "${CMAKE_CURRENT_LIST_DIR}/src/trim.cpp")
target_link_libraries(trim_exe PRIVATE trim)
set_target_properties(trim_exe PROPERTIES RUNTIME_OUTPUT_NAME "trim")

## =====================================================================================================
## Test target
## =====================================================================================================

if (TRIM_ENABLE_TESTS)
  add_library(test_options INTERFACE)

  if(MSVC)
    target_compile_options(test_options INTERFACE /W4)
  else()
    target_compile_options(test_options INTERFACE -Wall -Wextra -Wpedantic)
  endif()

  add_executable(driver "${CMAKE_CURRENT_LIST_DIR}/test/unit/driver.cpp")
  target_link_libraries(driver PRIVATE trim)
  target_link_libraries(driver PRIVATE test_options)
  target_include_directories(driver PRIVATE "${CMAKE_CURRENT_LIST_DIR}/test/")
  set_target_properties(driver PROPERTIES RUNTIME_OUTPUT_DIRECTORY unit)
endif()

if (TRIM_ENABLE_DOCS)

# TODO

endif()
