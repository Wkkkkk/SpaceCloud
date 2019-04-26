project(render)

find_package(OpenSceneGraph REQUIRED)
find_package(osgEarth REQUIRED)
find_package(PCL REQUIRED)

file(GLOB SOURCES_CXX *.cpp *.h)
include_directories(
        ${OPENSCENEGRAPH_INCLUDE_DIRS}
        ${OSGEARTH_INCLUDE_DIRS}
        ${PCL_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIR}
)
link_directories(
        ${OPENSCENEGRAPH_LIBRARY_DIRS}
        ${PCL_LIBRARY_DIRS}
)
add_definitions(${PCL_DEFINITIONS})

add_library(${PROJECT_NAME} ${SOURCES_CXX})
target_link_libraries(${PROJECT_NAME}
        ${OPENSCENEGRAPH_LIBRARIES}
        ${OSGEARTH_LIBRARIES}
        ${PCL_LIBRARIES}
        )
qt5_use_modules(${PROJECT_NAME} Sql Gui Widgets OpenGL Network Concurrent)