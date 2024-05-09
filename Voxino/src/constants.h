#pragma once

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <stdlib.h>

// #define DISABLE_IMGUI
// #define IS_TRACY_ENABLED TRUE
#define IS_MINITRACE_ENABLED FALSE
// #define TRACY_ON_DEMAND TRUE
// #define TRACY_AUTOMATIC_MEMORY_PROFILING
// #define TRACY_COLLECT_THUMBNAILS
// #define ENABLE_TRACY_MARKERS
// #define PLOT_AVERAGE_FPS

#define CHUNK_CONTAINER_RADIUS 1;
#define BLOCK_PER_DIMENSION_IN_CHUNK 64;
constexpr static auto IS_MINITRACE_COLLECTING_AT_START = false;