cmake_minimum_required(VERSION 3.16)
project(VIASEGURA VERSION 2.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

if(ENABLE_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(viasegura_core geo.c dataset.c query.c csv.c)
target_include_directories(viasegura_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(VIASEGURA main.c)
target_link_libraries(VIASEGURA PRIVATE viasegura_core)

if(UNIX)
    target_link_libraries(viasegura_core PUBLIC m)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    foreach(target viasegura_core VIASEGURA)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endforeach()
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(CTest)
if(BUILD_TESTING)
    add_executable(test_geo tests/test_geo.c)
    target_link_libraries(test_geo PRIVATE viasegura_core)
    add_test(NAME geo COMMAND test_geo)

    add_executable(test_dataset tests/test_dataset.c)
    target_link_libraries(test_dataset PRIVATE viasegura_core)
    add_test(NAME dataset_and_query COMMAND test_dataset ${CMAKE_CURRENT_SOURCE_DIR}/data/videovigilancia.txt)

    add_test(
        NAME cli_nearby
        COMMAND VIASEGURA --data ${CMAKE_CURRENT_SOURCE_DIR}/data/videovigilancia.txt
            --lat 40.4168 --lon -3.7038 --radius 3
    )
    add_test(
        NAME cli_invalid_coordinates
        COMMAND VIASEGURA --data ${CMAKE_CURRENT_SOURCE_DIR}/data/videovigilancia.txt
            --lat 91 --lon -3.7038 --radius 3
    )
    set_tests_properties(cli_invalid_coordinates PROPERTIES WILL_FAIL TRUE)
endif()
