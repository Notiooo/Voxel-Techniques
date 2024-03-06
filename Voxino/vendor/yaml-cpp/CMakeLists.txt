message(STATUS "Fetching Yaml-Cpp...")

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 76dc6715734295ff1866bfc32872ff2278258fc8
)
FetchContent_MakeAvailable(yaml-cpp)

message(STATUS "Yaml-Cpp Fetched!")