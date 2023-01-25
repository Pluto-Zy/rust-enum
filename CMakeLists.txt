cmake_minimum_required(VERSION 3.14)
project(rust_enum)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE
            PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(RUST_ENUM_ENABLE_TESTING "Enable testing of the rust_enum project." ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ClangFormat)
include(AddTarget)

add_subdirectory(src)

if (RUST_ENUM_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(unittest)
endif()
