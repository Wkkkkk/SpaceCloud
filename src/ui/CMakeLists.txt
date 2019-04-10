project(${PROJECT_NAME})

file(GLOB SOURCES_CXX *.cpp *.ui *.h *.qrc)

file(GLOB TRANSLATION_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/tr/lpd_zh.ts)

qt5_add_translation(TRANSLATION_QM ${TRANSLATION_FILES})
add_custom_target(translations
        DEPENDS ${TRANSLATION_QM})

add_custom_target(copy_ts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${TRANSLATION_QM} ${EXECUTABLE_OUTPUT_PATH}/tr/lpd_zh.qm
        DEPENDS ${TRANSLATION_QM})

add_custom_target(copy_model
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/model/* ${EXECUTABLE_OUTPUT_PATH}/model/
        )

add_custom_target(copy_images
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/images/reflect.rgb ${EXECUTABLE_OUTPUT_PATH}/model/Images/reflect.rgb
        )

add_custom_target(copy_manual
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Manual.pdf ${EXECUTABLE_OUTPUT_PATH}/Manual.pdf
        )

add_custom_target(copy_fonts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/fonts/arial.ttf ${EXECUTABLE_OUTPUT_PATH}/fonts/arial.ttf
        )

add_executable(${PROJECT_NAME} ${SOURCES_CXX})
add_dependencies(${PROJECT_NAME} copy_ts copy_model copy_images copy_manual copy_fonts)

target_link_libraries(${PROJECT_NAME} core render)
qt5_use_modules(${PROJECT_NAME} Sql Gui Widgets OpenGL Network Concurrent)