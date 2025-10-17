add_library(scanner_core SHARED
        src/scanner.cpp
        src/csv_loader.cpp
        src/md5.cpp
)


target_include_directories(scanner_core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)


if (WIN32)
    target_compile_definitions(scanner_core PRIVATE SCANNER_CORE_EXPORTS)
endif()


set_target_properties(scanner_core PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)