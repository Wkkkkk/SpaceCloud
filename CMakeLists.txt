cmake_minimum_required(VERSION 3.5)
project(SpaceCloud)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_PREFIX_PATH /usr/local/lib64:/usr/local/Qt-5.11.1/lib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CXX_FLAGS
        -g
        -Wall
        -Wextra
        #        -Werror
#        -Wconversion
        -Wno-unused-parameter
        #        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
        -march=native
        -rdynamic
        )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

find_package(Qt5LinguistTools)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

IF(BUILD_DOCUMENTATION)
    IF(NOT DOXYGEN_FOUND)
        MESSAGE(FATAL_ERROR "Doxygen is needed to build the documentation.")
    ENDIF()

    SET(doxyfile_in ${PROJECT_SOURCE_DIR}/Doxyfile.in)
    SET(doxyfile ${PROJECT_BINARY_DIR}/Doxyfile)

    # copy and replace the variables
    configure_file(${doxyfile_in} ${doxyfile})

    # use doxygen to generate the doc
    add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

    #INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/html DESTINATION share/doc)
ENDIF()

add_subdirectory(src)