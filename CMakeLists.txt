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

# ------------------------------------------------------------------------------
# Command-line application
# ------------------------------------------------------------------------------
file(GLOB_RECURSE FINANCE_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(finance_core
    ${FINANCE_CORE_SOURCES}
)

target_include_directories(finance_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(finance_core PRIVATE
    FINANCE_CORE_VERSION="${PROJECT_VERSION}"
)

# ------------------------------------------------------------------------------
# Unit Testing (doctest)
# ------------------------------------------------------------------------------
option(BUILD_TESTS "Build unit test suite for finance-core" ON)

if(BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.12
    )
    FetchContent_MakeAvailable(doctest)

    file(GLOB_RECURSE FINANCE_CORE_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    set(FINANCE_CORE_TEST_IMPLEMENTATION_SOURCES ${FINANCE_CORE_SOURCES})
    list(REMOVE_ITEM FINANCE_CORE_TEST_IMPLEMENTATION_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    )

    add_executable(finance_core_tests
        ${FINANCE_CORE_TEST_SOURCES}
        ${FINANCE_CORE_TEST_IMPLEMENTATION_SOURCES}
    )

    target_include_directories(finance_core_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_definitions(finance_core_tests PRIVATE
        FINANCE_CORE_VERSION="${PROJECT_VERSION}"
    )

    target_link_libraries(finance_core_tests PRIVATE doctest::doctest)

    include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
    doctest_discover_tests(finance_core_tests)
endif()