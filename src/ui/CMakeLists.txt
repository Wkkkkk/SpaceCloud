project(${PROJECT_NAME})

file(GLOB SOURCES_CXX *.cpp *.ui *.h *.qrc *.rc)

file(GLOB TRANSLATION_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/tr/lpd_zh.ts)

qt5_add_translation(TRANSLATION_QM ${TRANSLATION_FILES})
add_custom_target(translations
        DEPENDS ${TRANSLATION_QM})

add_custom_target(copy_ts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${TRANSLATION_QM} ${COPY_OUTPUT_PATH}/tr/lpd_zh.qm
        DEPENDS ${TRANSLATION_QM})

add_custom_target(copy_images
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/images/moon_1024x512.jpg ${COPY_OUTPUT_PATH}/moon_1024x512.jpg
        )

add_custom_target(copy_manual
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Manual.pdf ${COPY_OUTPUT_PATH}/Manual.pdf
        )

add_custom_target(copy_fonts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/fonts/arial.ttf ${COPY_OUTPUT_PATH}/fonts/arial.ttf
        )

add_custom_target(copy_earth
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/earth/gisms.earth ${COPY_OUTPUT_PATH}/earth/gisms.earth
        )

IF (WIN32)
    #set(PLATFORM WIN32)
ENDIF ()

add_executable(${PROJECT_NAME} ${PLATFORM} ${SOURCES_CXX})
add_dependencies(${PROJECT_NAME} copy_ts copy_images copy_manual copy_fonts copy_earth)

target_link_libraries(${PROJECT_NAME} core render
        ${Boost_LIBRARIES}
        ${OPENSCENEGRAPH_LIBRARIES}
        ${OSGEARTH_LIBRARIES}
        ${PCL_LIBRARIES}
        )
qt5_use_modules(${PROJECT_NAME} Sql Gui Widgets OpenGL Network Concurrent)