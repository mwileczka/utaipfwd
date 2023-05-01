cmake_minimum_required (VERSION 3.5)

project (utaipfwd)

add_executable(utaipfwd)

target_sources(utaipfwd PRIVATE utaipfwd.c minmea.c taip.c ini.c)

target_include_directories (utaipfwd PRIVATE ${PROJECT_SOURCE_DIR})

add_custom_command(TARGET utaipfwd POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/utaipfwd.ini ${CMAKE_CURRENT_BINARY_DIR}
)