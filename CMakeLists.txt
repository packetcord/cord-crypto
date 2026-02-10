cmake_minimum_required(VERSION 3.20)
project(cord_crypto  LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This is an INTERFACE library: no sources, just headers
add_library(cord_crypto INTERFACE)

# Assume public headers live under modules/cord-crypto/include
target_include_directories(cord_crypto INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

