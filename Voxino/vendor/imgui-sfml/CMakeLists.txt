message(STATUS "Fetching ImGui-SFML...")

FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/Notiooo/imgui-sfml-opengl3
        GIT_TAG a9de4a076ce5e67a940ca1e7a79b9a1819d6ce95
)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)

FetchContent_MakeAvailable(imgui-sfml)

message(STATUS "ImGui-SFML Fetched!")