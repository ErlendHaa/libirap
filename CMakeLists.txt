cmake_minimum_required(VERSION 3.5)
project(irap LANGUAGES C CXX)

include(CheckIncludeFile)
include(CheckFunctionExists)
include(GNUInstallDirs)
include(CTest)
include(TestBigEndian)

option(BUILD_APPS "Build applications"  OFF)

set(CMAKE_C_STANDARD   99)
set(CMAKE_CXX_STANDARD 11)

test_big_endian(BIG_ENDIAN)

check_include_file(getopt.h       HAVE_GETOPT_H)
check_function_exists(getopt_long HAVE_GETOPT_LONG)

if (NOT MSVC)
    # assuming gcc-style options
    # add warnings in debug mode
    list(APPEND warnings-c++ -Wall
                             -Wextra
                             -pedantic
                             -Wformat-nonliteral
                             -Wcast-align
                             -Wpointer-arith
                             -Wmissing-declarations
                             -Wcast-qual
                             -Wwrite-strings
                             -Wchar-subscripts
                             -Wredundant-decls
    )
endif()

add_subdirectory(external/endianness)

add_library(irap)
add_library(irap::irap ALIAS irap)

target_sources(irap PRIVATE src/bin/parse.cpp)
target_include_directories(irap
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
    PRIVATE $<TARGET_PROPERTY:endianness::endianness,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_options(irap
    BEFORE
    PRIVATE
        $<$<CONFIG:Debug>:${warnings-c++}>
        $<$<CONFIG:RelWithDebInfo>:${warnings-c++}>
        $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
)

target_compile_definitions(irap
    PRIVATE
    $<$<BOOL:${BIG_ENDIAN}>:HOST_BIG_ENDIAN>
    $<$<NOT:$<BOOL:${BIG_ENDIAN}>>:HOST_LITTLE_ENDIAN>
)

install(
    TARGETS irap
    EXPORT irap-export
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION include)
install(
    EXPORT
        irap-export
    DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/irap/cmake
    FILE
        irap-config.cmake
    NAMESPACE
        irap::
)

export(
    TARGETS
        irap
    FILE
        irap-config.cmake
    NAMESPACE
        irap::
)

add_subdirectory(applications)

if (NOT BUILD_TESTING)
    return()
endif()

add_subdirectory(external/catch2)

add_executable(test-bin-parse tests/bin/parse.cpp)
target_compile_features(test-bin-parse PUBLIC cxx_std_11)
target_compile_options(test-bin-parse
    BEFORE
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
)

target_link_libraries(test-bin-parse
    PRIVATE
        catch2
        irap::irap
)
add_test(NAME test-bin-parse COMMAND test-bin-parse)
