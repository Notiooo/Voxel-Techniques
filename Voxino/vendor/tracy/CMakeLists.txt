message(STATUS "Fetching Tracy...")

option(DOWNLOAD_AND_RUN_TRACY_PROFILER "Downloads and runs tracy profiler from GH" ON)

set(TRACY_VERSION_TAG v0.10)

FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG        ${TRACY_VERSION_TAG}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
option(TRACY_NO_SAMPLING "" ON)
option(TRACY_NO_SYSTEM_TRACING "" ON)
option(TRACY_ON_DEMAND "" ON)
option(TRACY_ONLY_LOCALHOST "" ON)
option(TRACY_TIMER_FALLBACK "" ON)
FetchContent_MakeAvailable(tracy)
message(STATUS "Tracy Fetched!")

if(WIN32 AND DOWNLOAD_AND_RUN_TRACY_PROFILER)
    string(SUBSTRING ${TRACY_VERSION_TAG} 1 -1 TRACY_STRIPPED_VERSION_TAG)
    set(TRACY_ARCHIVE_NAME Tracy-${TRACY_STRIPPED_VERSION_TAG}.7z)
    # Check if the archive already exists before downloading
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/bin/tracy/${TRACY_ARCHIVE_NAME}")
        message(STATUS "Downloading Tracy ${TRACY_VERSION_TAG}")
        file(DOWNLOAD https://github.com/wolfpld/tracy/releases/download/${TRACY_VERSION_TAG}/${TRACY_ARCHIVE_NAME} ${CMAKE_BINARY_DIR}/bin/tracy/${TRACY_ARCHIVE_NAME})
    else()
        message(STATUS "${TRACY_ARCHIVE_NAME} already downloaded.")
    endif()
    find_program(SEVENZIP_EXECUTABLE 7z.exe)
    if(SEVENZIP_EXECUTABLE)
        # Check if the extraction target folder already exists to avoid re-extraction
        if(NOT EXISTS ${TRACY_EXECUTABLE})
            execute_process(COMMAND ${SEVENZIP_EXECUTABLE} x ${CMAKE_BINARY_DIR}/bin/tracy/${TRACY_ARCHIVE_NAME} -o${CMAKE_BINARY_DIR}/bin/tracy/)
            set(TRACY_EXECUTABLE "${CMAKE_BINARY_DIR}/bin/tracy/Tracy.exe" CACHE STRING "Path to executable ot Tracy.exe")
        else()
            message(STATUS "Tracy ${TRACY_VERSION_TAG} already extracted.")
        endif()
    else()
        message(STATUS "7zip not found - extract ${CMAKE_BINARY_DIR}/bin/tracy/${TRACY_ARCHIVE_NAME} manually or provide the path to 7z via CMAKE_PROGRAM_PATH")
    endif()
endif()
