project(core)

file(GLOB SOURCES_CXX *.cpp *.h)

IF (UNIX)
    set(PTHREAD_LIB pthread)
ENDIF ()

add_library(${PROJECT_NAME} ${SOURCES_CXX})
target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIB} ${Boost_LIBRARIES})