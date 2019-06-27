cmake_minimum_required(VERSION 3.5)
project(SpaceCloud)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release CACHE STRING "set build type to Release")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(COPY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
#set(CMAKE_CXX_FLAGS "-DBOOST_LOG_DYN_LINK")
set(Boost_USE_STATIC_LIBS ON)

#output setting for windows compatibility
if(MSVC)
    set(COPY_OUTPUT_PATH ${COPY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32")  # for #include<gl/gl.h> bugs
    add_definitions("/EHsc") # boost::throw_exception bugs
endif(MSVC)

#find 3rdparty
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
message(STATUS "VCPKG_DIR:$ENV{VCPKG_DIR}")
message(STATUS "EXECUTABLE_OUTPUT_PATH :${EXECUTABLE_OUTPUT_PATH}")

find_package(Qt5LinguistTools)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)

find_package(PCL REQUIRED) # better placed before boost
add_definitions(${PCL_DEFINITIONS})
find_package(OpenSceneGraph REQUIRED)
find_package(osgEarth REQUIRED)
find_package(Boost COMPONENTS thread date_time iostreams serialization chrono atomic filesystem system log REQUIRED)

message(STATUS "boost:${Boost_LIBRARIES}")
include_directories(
        ${PCL_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${OPENSCENEGRAPH_INCLUDE_DIRS}
        ${OSGEARTH_INCLUDE_DIRS}
)

link_directories(
        ${PCL_LIBRARY_DIRS}
        ${Boost_LIBRARY_DIRS}
        ${OPENSCENEGRAPH_LIBRARY_DIRS}
)

add_subdirectory(src)