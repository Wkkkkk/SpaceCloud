project(render)

find_package(OpenSceneGraph REQUIRED)
find_package(osgEarth REQUIRED)

file(GLOB SOURCES_CXX *.cpp *.h)
include_directories(
        ${OPENSCENEGRAPH_INCLUDE_DIRS}
        ${OSGEARTH_INCLUDE_DIRS}
)
link_directories(${OPENSCENEGRAPH_LIBRARY_DIRS})

add_library(${PROJECT_NAME} ${SOURCES_CXX})
target_link_libraries(${PROJECT_NAME}
        ${OPENSCENEGRAPH_LIBRARIES}
        ${OSGEarth_LIBRARIES}
        )
qt5_use_modules(${PROJECT_NAME} Sql Gui Widgets OpenGL Network Concurrent)