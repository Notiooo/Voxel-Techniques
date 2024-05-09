#pragma once
#include "Tracy.hpp"
#include "TracyOpenGL.hpp"
#include "constants.h"
#include "profiler_memory_tracking.h"

#if defined(UNIT_TEST)
    #undef MTR_ENABLED
    #undef TRACY_ENABLE
#else

    #if IS_TRACY_ENABLED
        #define TRACY_ENABLE
    #else
        #undef TRACY_ENABLE
    #endif

    #if IS_MINITRACE_ENABLED
        #define MTR_ENABLED
    #else
        #undef MTR_ENABLED
    #endif

#endif

#ifdef MTR_ENABLED

    #define __MINITRACE_FILENAME__                                                                 \
        (strstr(__FILE__, "Voxino/src/")                                                           \
             ? strstr(__FILE__, "Voxino/src/") + strlen("Voxino/src/")                             \
             : (strstr(__FILE__, "Voxino\\src\\")                                                  \
                    ? strstr(__FILE__, "Voxino\\src\\") + strlen("Voxino\\src\\")                  \
                    : __FILE__))


    #define MINITRACE_COLLECT_FUNCTION                                                             \
        auto __minitrace_macros_location = std::source_location::current();                        \
        MTR_SCOPE(__MINITRACE_FILENAME__, __minitrace_macros_location.function_name())

    #define MINITRACE_COLLECT_FUNCTION_CUSTOM(custom_name)                                         \
        MTR_SCOPE(__MINITRACE_FILENAME__, custom_name)

#else
    #define MINITRACE_COLLECT_FUNCTION (void)0
    #define MINITRACE_COLLECT_FUNCTION_CUSTOM(custom_name) (void)0
#endif

#ifdef ENABLE_TRACY_MARKERS

    #define GPU_COLLECT                                                                            \
        TracyGpuCollect;                                                                           \
        (void)0

    #define FRAME_MARKER                                                                           \
        FrameMark;                                                                                 \
        (void)0

    #define MEASURE_SCOPE                                                                          \
        MINITRACE_COLLECT_FUNCTION;                                                                \
        ZoneScoped;                                                                                \
        (void)0

    #define MEASURE_SCOPE_WITH_GPU                                                                 \
        MINITRACE_COLLECT_FUNCTION;                                                                \
        ZoneScoped;                                                                                \
        TracyGpuZone(__FUNCTION__);                                                                \
        (void)0

    #define MEASURE_SCOPE_CUSTOM(name)                                                             \
        MINITRACE_COLLECT_FUNCTION_CUSTOM(name);                                                   \
        ZoneScopedN(name);                                                                         \
        (void)0

    #define MEASURE_SCOPE_WITH_GPU_CUSTOM(name)                                                    \
        MINITRACE_COLLECT_FUNCTION_CUSTOM(name);                                                   \
        ZoneScopedN(name);                                                                         \
        TracyGpuZone(name);                                                                        \
        (void)0


    #define TracyMessageAuto(msg) TracyMessage(msg, strlen(msg))
    #define TracyMessageAutoC(msg, color) TracyMessageC(msg, strlen(msg), color)
    #define TracyMessageAutoL(msg) TracyMessageL(msg, strlen(msg))
    #define TracyMessageAutoLC(msg, color) TracyMessageLC(msg, strlen(msg), color)

#else
    #define GPU_COLLECT
    #define FRAME_MARKER
    #define MEASURE_SCOPE
    #define MEASURE_SCOPE_WITH_GPU
    #define MEASURE_SCOPE_CUSTOM(name)
    #define MEASURE_SCOPE_WITH_GPU_CUSTOM(name)
    #define TracyMessageAuto(msg)
    #define TracyMessageAutoC(msg, color)
    #define TracyMessageAutoL(msg)
    #define TracyMessageAutoLC(msg, color)

#endif
