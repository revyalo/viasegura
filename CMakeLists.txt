cmake_minimum_required(VERSION 3.16)
project(VIASEGURA C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(VIASEGURA main.c geo.c)

if(UNIX)
    target_link_libraries(VIASEGURA PRIVATE m)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
