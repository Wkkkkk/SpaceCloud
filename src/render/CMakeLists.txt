project(render)
if (WIN32)
    set(CMAKE_CXX_STANDARD 14) # flann bug(std::random_shuffle is deprecated after c++14)
endif ()

file(GLOB SOURCES_CXX *.cpp *.h)

add_library(${PROJECT_NAME} ${SOURCES_CXX})
qt5_use_modules(${PROJECT_NAME} Sql Gui Widgets OpenGL Network Concurrent)