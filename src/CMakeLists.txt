set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

SET(MODULE_DIRS
        core
        render
        ui
        )

FOREACH (MODULE_DIR ${MODULE_DIRS})
    include_directories(${MODULE_DIR})
    add_subdirectory(${MODULE_DIR})
ENDFOREACH (MODULE_DIR)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "Building test......")
    add_subdirectory(test)
endif()
