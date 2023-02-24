project(irap-applications)

if (NOT BUILD_APPS)
    return ()
endif ()

if( NOT HAVE_GETOPT_H OR NOT HAVE_GETOPT_LONG )
    message(WARNING "Could not find getopt. Not building applications.")
    return()
endif ()

set(CMAKE_CXX_STANDARD 11)


add_executable(irap-cat cat.cpp)
target_link_libraries(irap-cat irap::irap)
target_compile_options(irap-cat BEFORE
    PRIVATE 
        $<$<CONFIG:Debug>:${warnings-c++}>
        $<$<CONFIG:RelWithDebInfo>:${warnings-c++}>
)

install(TARGETS irap-cat DESTINATION ${CMAKE_INSTALL_BINDIR})

if (NOT BUILD_TESTING)
    return()
endif()

set(binfile ${CMAKE_SOURCE_DIR}/data/bingrid.bin)
set(test ${CMAKE_CURRENT_SOURCE_DIR}/test)
add_test(NAME cat.arg.t-binary    COMMAND irap-cat -t     binary ${binfile})
add_test(NAME cat.arg.type-binary COMMAND irap-cat --type binary ${binfile})

add_test(NAME cat.arg.help          COMMAND irap-cat --help)
add_test(NAME cat.fail.noirap       COMMAND irap-cat --type binary ${test}/cat-bin.output)
add_test(NAME cat.fail.nofile       COMMAND irap-cat --type not-exist)
add_test(NAME cat.fail.noarg        COMMAND irap-cat)
add_test(NAME cat.fail.notype.type  COMMAND irap-cat ${binfile})
add_test(NAME cat.fail.invalid.type COMMAND irap-cat --type invalid ${binfile})

set_tests_properties(
    cat.fail.noirap
    cat.fail.nofile
    cat.fail.noarg
    cat.fail.notype.type
    cat.fail.invalid.type
        PROPERTIES WILL_FAIL ON
)

add_custom_target(test-app-output
    ALL
    DEPENDS cat-bin.out
)

add_custom_command(
    OUTPUT cat-bin.out
    COMMENT "running applications for output comparison"
    DEPENDS irap-cat 
            test/cat-bin.output
    COMMAND irap-cat -t binary ${binfile} > cat-bin.out
)

add_test(NAME cat-bin.output
    COMMAND ${CMAKE_COMMAND} -E compare_files ${test}/cat-bin.output cat-bin.out
)
