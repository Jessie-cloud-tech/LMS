cmake_minimum_required(VERSION 3.16)
project(ModeratedLMS LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(LMS_BUILD_TESTS "Build the automated tests" ON)
option(LMS_COVERAGE "Measure test coverage (GCC/Clang only)" OFF)

include(FetchContent)

# SQLite
FetchContent_Declare(sqlite_amalgamation
    URL https://www.sqlite.org/2024/sqlite-amalgamation-3460000.zip
)

FetchContent_GetProperties(sqlite_amalgamation)

if(NOT sqlite_amalgamation_POPULATED)
    FetchContent_Populate(sqlite_amalgamation)
endif()

add_library(sqlite3_lib STATIC
    ${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c
)

target_include_directories(sqlite3_lib PUBLIC
    ${sqlite_amalgamation_SOURCE_DIR}
)

find_package(Threads REQUIRED)

target_link_libraries(sqlite3_lib PUBLIC
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

if(NOT MSVC)
    target_compile_options(sqlite3_lib PRIVATE -w)
endif()


# libsodium
set(SODIUM_DISABLE_TESTS ON CACHE BOOL "" FORCE)

FetchContent_Declare(sodium
    GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
    GIT_TAG master
)

FetchContent_MakeAvailable(sodium)


# Core application source files
file(GLOB CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_library(lms_core STATIC ${CORE_SOURCES})

target_include_directories(lms_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(lms_core PUBLIC
    sqlite3_lib
    sodium
)

if(MSVC)
    target_compile_definitions(lms_core PUBLIC _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(lms_core PRIVATE -Wall -Wextra)
endif()

if(LMS_COVERAGE AND NOT MSVC)
    target_compile_options(lms_core PUBLIC --coverage -O0 -g)
    target_link_options(lms_core PUBLIC --coverage)
endif()


# Main application
add_executable(lms main.cpp)
target_link_libraries(lms PRIVATE lms_core)


# GoogleTest
if(LMS_BUILD_TESTS)

    enable_testing()

    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    )

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)


    # Unit, integration and service tests
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/tests/unit/*.cpp
        ${CMAKE_SOURCE_DIR}/tests/integration/*.cpp
        ${CMAKE_SOURCE_DIR}/tests/services/*.cpp
    )

    if(TEST_SOURCES)
        add_executable(lms_tests ${TEST_SOURCES})

        target_include_directories(lms_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/tests
        )

        target_link_libraries(lms_tests PRIVATE
            lms_core
            GTest::gtest_main
        )

        add_test(NAME unit_integration_service_tests
                 COMMAND lms_tests)
    else()
        message(WARNING "No unit, integration or service test .cpp files were found.")
    endif()


    # C++ system tests
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/system/systemTest.cpp")

        add_executable(lms_system_tests
            tests/system/systemTest.cpp
        )

        target_include_directories(lms_system_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/tests
        )

        target_link_libraries(lms_system_tests PRIVATE
            lms_core
            GTest::gtest_main
        )

        add_test(NAME system_tests COMMAND lms_system_tests)

    else()
        message(WARNING "tests/system/systemTest.cpp was not found.")
    endif()

endif()