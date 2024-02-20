#pragma once

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <stdlib.h>

#ifndef _DEBUG
    #undef MTR_ENABLED
#endif

/**
 * \brief This will remove 2D drawn by SFML and also the ImGUI
 */
// #define DISABLE_SFML_GRAPHICS_MODULE
// #define DISABLE_IMGUI

constexpr static auto IS_MINITRACE_COLLECTING_AT_START = false;