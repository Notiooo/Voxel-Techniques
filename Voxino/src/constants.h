#pragma once

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <stdlib.h>

/**
 * \brief This will remove 2D drawn by SFML and also the ImGUI
 */
// #define DISABLE_IMGUI
#define IS_TRACY_ENABLED TRUE
#define IS_MINITRACE_ENABLED FALSE

constexpr static auto IS_MINITRACE_COLLECTING_AT_START = false;