message(STATUS "Fetching SFML...")

FetchContent_Declare(
        sfml
        GIT_REPOSITORY "https://github.com/SFML/SFML"
        GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab # 2.5.1
)

# No need to build network modules
set(SFML_BUILD_NETWORK FALSE)
set(SFML_BUILD_GRAPHICS FALSE)

FetchContent_MakeAvailable(sfml)

message(STATUS "SFML Fetched!")