include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2

)
FetchContent_MakeAvailable(googletest)

add_executable(scanner_tests test_core.cpp)
target_link_libraries(scanner_tests PRIVATE scanner_core gtest gtest_main)


add_dependencies(scanner_tests scanner_core)
enable_testing()
add_test(NAME scanner_tests COMMAND $<TARGET_FILE:scanner_tests>)
set_tests_properties(scanner_tests PROPERTIES WORKING_DIRECTORY $<TARGET_FILE_DIR:scanner_tests>)


add_custom_command(TARGET scanner_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying scanner_core.dll next to scanner_tests.exe"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:scanner_core>
        $<TARGET_FILE_DIR:scanner_tests>
)
