cmake_minimum_required(VERSION 3.24)

project(VoxinoBenchmarks LANGUAGES CXX)

include_directories(../src)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

configure_file("CMakeLists.txt.in" "${CMAKE_BINARY_DIR}/googlebenchmark-download/CMakeLists.txt")
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googlebenchmark-download"
        )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googlebenchmark-download"
        )

#set(benchmark_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_BINARY_DIR}/googlebenchmark-src"
        "${CMAKE_BINARY_DIR}/googlebenchmark-build"
        )

if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${benchmark_SOURCE_DIR}/include"
            "${gmock_SOURCE_DIR}/include"
            )
endif ()

include(benchmarks/CMakeLists.txt)
include(../tests/mocks/CMakeLists.txt)
include(../tests/utils/CMakeLists.txt)
include_directories(benchmarks/src)
include_directories(../tests/mocks/src)
include_directories(../tests/utils/src)
list(TRANSFORM Benchmark_Sources PREPEND benchmarks/)
list(TRANSFORM Mocks_Sources PREPEND ../tests/mocks/)
list(TRANSFORM Utils_Sources PREPEND ../tests/utils/)

add_executable(VoxinoBenchmarks
        ${Benchmark_Sources}
        ${Mocks_Sources}
        ${Utils_Sources}
        main.cpp
        )

#target_compile_definitions(VoxinoBenchmarks PUBLIC BENCHMARK_LINKED_AS_SHARED_LIBRARY)
target_link_libraries(VoxinoBenchmarks PRIVATE VoxinoSrc)
target_link_libraries(VoxinoBenchmarks PRIVATE benchmark benchmark_main)
target_link_libraries(VoxinoBenchmarks PRIVATE gmock gmock_main)

# Copy libraries

add_custom_command(TARGET VoxinoBenchmarks POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:benchmark>
        $<TARGET_FILE:gmock>
        $<TARGET_FILE:benchmark_main>
        $<TARGET_FILE:gmock_main>
        $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE:sfml-audio>
        $<TARGET_FILE:spdlog>
        $<TARGET_FILE_DIR:VoxinoBenchmarks>
        )

# Remove previous application resources

add_custom_command(TARGET VoxinoBenchmarks PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        $<TARGET_FILE_DIR:VoxinoBenchmarks>/resources
        )

# Copy application resources

add_custom_command(TARGET VoxinoBenchmarks POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:VoxinoBenchmarks>/resources
        )