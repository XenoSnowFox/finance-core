cmake_minimum_required(VERSION 3.22)
project(finance_core VERSION 1.0.0 LANGUAGES CXX C)

# Force C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Warning Flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
endif()

# ------------------------------------------------------------------------------
# Dependencies (FetchContent)
# ------------------------------------------------------------------------------
include(FetchContent)

# Export include paths ($<BUILD_INTERFACE> for local builds, $<INSTALL_INTERFACE> for installs)
target_include_directories(finance_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Namespace alias for downstream consumers (finance::core)
add_library(finance::core ALIAS finance_core)

# ------------------------------------------------------------------------------
# Unit Testing (Catch2)
# ------------------------------------------------------------------------------
option(BUILD_TESTS "Build unit test suite for finance-core" ON)

if(BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.16.1
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(core_unit_tests
        tests/test_transaction.cpp
    )

    target_link_libraries(core_unit_tests PRIVATE
        finance::core
        Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(core_unit_tests)
endif()