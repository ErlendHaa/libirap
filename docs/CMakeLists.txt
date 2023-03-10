if (NOT BUILD_DOCS)
    return()
endif()

cmake_minimum_required(VERSION 3.5)
project(libirap-doc)

find_package(Doxygen REQUIRED)
find_package(PythonInterp REQUIRED)

get_filename_component(ROOT ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(DOXYGEN_INPUT_DIR   ${ROOT}/include/irap)
set(DOXYGEN_OUTPUT_DIR  ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYFILE_IN         ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set(SPHINX_SOURCE       ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD        ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

add_custom_target(doxygen
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
    COMMAND
        ${DOXYGEN_EXECUTABLE} Doxyfile ${DOXYFILE_OUT}
    COMMENT
        "Generating docs with doxygen"
    VERBATIM
)

add_custom_target(sphinx
    COMMAND
        ${PYTHON_EXECUTABLE} -m sphinx -b html
        ${SPHINX_SOURCE} ${SPHINX_BUILD}
        -Dbreathe_projects.libirap=${DOXYGEN_OUTPUT_DIR}/xml
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
        doxygen
    COMMENT
        "Generating docs with sphinx"
    VERBATIM
)

add_custom_target(doc ALL DEPENDS sphinx)
