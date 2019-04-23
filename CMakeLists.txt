cmake_minimum_required(VERSION 3.5)
project(SpaceCloud)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release CACHE STRING "set build type to Release")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(COPY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS "-DBOOST_LOG_DYN_LINK")

#output setting for windows compatibility
IF (WIN32)
    set(COPY_OUTPUT_PATH ${COPY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32")  # for #include<gl/gl.h> bugs
ENDIF ()

#find 3rdparty
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
message(STATUS "CMAKE_PREFIX_PATH:$ENV{CMAKE_PREFIX_PATH}")
message(STATUS "LD_LIBRARY_PATH:$ENV{LD_LIBRARY_PATH}")
message(STATUS "VCPKG_DIR:$ENV{VCPKG_DIR}")
message(STATUS "EXECUTABLE_OUTPUT_PATH :${EXECUTABLE_OUTPUT_PATH}")

IF (UNIX)
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}:/usr/local/Qt-5.11.1/lib")  # for local Qt5.11

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
    string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS};${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")

    option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation")
    IF (BUILD_DOCUMENTATION)
        include(UseBreathe)

        add_breathe_doc(
                SOURCE_DIR
                ${CMAKE_CURRENT_SOURCE_DIR}/docs
                BUILD_DIR
                ${CMAKE_CURRENT_BINARY_DIR}/_build
                CACHE_DIR
                ${CMAKE_CURRENT_BINARY_DIR}/_doctrees
                HTML_DIR
                ${CMAKE_CURRENT_BINARY_DIR}/html
                DOXY_FILE
                ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
                CONF_FILE
                ${CMAKE_CURRENT_SOURCE_DIR}/docs/conf.py.in
                TARGET_NAME
                docs
                COMMENT
                "HTML documentation"
        )
    ENDIF ()
ENDIF ()

find_package(Qt5LinguistTools)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Boost COMPONENTS log thread date_time filesystem system REQUIRED)

add_subdirectory(src)