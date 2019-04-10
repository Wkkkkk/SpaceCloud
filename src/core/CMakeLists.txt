project(core)

file(GLOB SOURCES_CXX *.cpp *.h)

add_library(${PROJECT_NAME} ${SOURCES_CXX})
target_link_libraries(${PROJECT_NAME} pthread)