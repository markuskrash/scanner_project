cmake_minimum_required(VERSION 3.16)
project(scanner_app)

add_executable(scanner_app main.cpp)

target_include_directories(scanner_app PRIVATE ${CMAKE_SOURCE_DIR}/src/core/include)

target_link_libraries(scanner_app PRIVATE scanner_core)

add_custom_command(
        TARGET scanner_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:scanner_core>
        $<TARGET_FILE_DIR:scanner_app>
        COMMENT "Copying scanner_core.dll next to scanner_app.exe..."
)

