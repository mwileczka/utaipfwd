cmake_minimum_required (VERSION 3.5)

project (rvtracker)

add_executable(rvtracker)

target_sources(rvtracker PRIVATE rvtracker.c minmea.c taip.c ini.c)

target_include_directories (rvtracker PRIVATE ${PROJECT_SOURCE_DIR})

add_custom_command(TARGET rvtracker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/rvtracker.ini ${CMAKE_CURRENT_BINARY_DIR}
)