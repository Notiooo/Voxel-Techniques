cmake_minimum_required(VERSION 3.24)

project(VoxinoSrc VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(CMakeLists_Sources.txt)

add_library(VoxinoSrc STATIC ${PROJECT_SOURCES})
target_precompile_headers(VoxinoSrc PUBLIC pch.h)

set(CUSTOM_INCLUDES_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_includes)
file(MAKE_DIRECTORY ${CUSTOM_INCLUDES_DIR})

set(CUSTOM_INCLUDES
        ${FETCHCONTENT_BASE_DIR}/minitrace-src/minitrace.h
        ${FETCHCONTENT_BASE_DIR}/stb-src/stb_image.h
)

foreach (HEADER ${CUSTOM_INCLUDES})
    configure_file(
            ${HEADER}
            ${CUSTOM_INCLUDES_DIR}
            COPYONLY
    )
endforeach ()

target_include_directories(VoxinoSrc PUBLIC
        ${FETCHCONTENT_BASE_DIR}/glew-src/include
        ${FETCHCONTENT_BASE_DIR}/result-src/include
        ${FETCHCONTENT_BASE_DIR}/entt-src/src
        ${FETCHCONTENT_BASE_DIR}/tracy-src/public/tracy
        ${FETCHCONTENT_BASE_DIR}/yaml-cpp-src/include
        ${FETCHCONTENT_BASE_DIR}/fastnoiselite-src/Cpp
        ${glm_SOURCE_DIR}
        ${CUSTOM_INCLUDES_DIR}
)


set_target_properties(sfml-audio PROPERTIES CXX_STANDARD 98 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)

target_link_libraries(VoxinoSrc
        PUBLIC
        sfml-audio
        sfml-window
        sfml-system
        ImGui-SFML::ImGui-SFML
        glew_s
        glm
        spdlog
        minitrace
        TracyClient
        yaml-cpp::yaml-cpp
)

# Copy libraries

add_custom_command(TARGET VoxinoSrc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE:sfml-audio>
        $<TARGET_FILE:spdlog>
        $<TARGET_FILE:minitrace>
        $<TARGET_FILE:TracyClient>
        $<TARGET_FILE:yaml-cpp::yaml-cpp>
        $<TARGET_FILE_DIR:VoxinoSrc>
)

# Copy OpenAL32.dll for SFML AUDIO
if (WIN32 AND SFML_BUILD_AUDIO)
    add_custom_command(TARGET VoxinoSrc POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${FETCHCONTENT_BASE_DIR}/sfml-src/extlibs/bin/x64/openal32.dll
            $<TARGET_FILE_DIR:VoxinoSrc>
    )
endif ()

# Remove previous engine resources

add_custom_command(TARGET VoxinoSrc PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        $<TARGET_FILE_DIR:VoxinoSrc>/resources
)

# Copy application resources

add_custom_command(TARGET VoxinoSrc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:VoxinoSrc>/resources
)

# Copy imgui config
add_custom_command(
        TARGET VoxinoSrc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env SOURCE_FILE="${CMAKE_SOURCE_DIR}/imgui.ini"
        DEST_DIR="$<TARGET_FILE_DIR:VoxinoSrc>"
        ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/copy_file_if_not_exist.cmake
        COMMENT "Ensure imgui.ini exists in the target directory."
)


add_executable(VoxinoApp main.cpp)

add_custom_command(TARGET VoxinoApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:VoxinoSrc>
        $<TARGET_FILE_DIR:VoxinoApp>
)

target_link_libraries(VoxinoApp PRIVATE VoxinoSrc)
