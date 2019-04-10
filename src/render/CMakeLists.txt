project(render)

find_package(OpenSceneGraph REQUIRED COMPONENTS osgGA osgViewer osgDB osgUtil osgText osg osgFX)

file(GLOB SOURCES_CXX *.cpp *.h)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
link_directories(${OPENSCENEGRAPH_LIBRARY_DIRS})
message(STATUS "find osg libraries: " ${OPENSCENEGRAPH_LIBRARIES})

add_library(${PROJECT_NAME} ${SOURCES_CXX})
target_link_libraries(${PROJECT_NAME} ${OPENSCENEGRAPH_LIBRARIES})
qt5_use_modules(${PROJECT_NAME} Sql Gui Widgets OpenGL Network Concurrent)