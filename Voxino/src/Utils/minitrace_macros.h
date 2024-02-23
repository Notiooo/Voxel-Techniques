#pragma once
#include <constants.h>
#include <filesystem>
#include <source_location>

#ifdef MTR_ENABLED

    #define __MINITRACE_FILENAME__                                                                 \
        (strstr(__FILE__, "Voxino/src/")                                                           \
             ? strstr(__FILE__, "Voxino/src/") + strlen("Voxino/src/")                             \
             : (strstr(__FILE__, "Voxino\\src\\")                                                  \
                    ? strstr(__FILE__, "Voxino\\src\\") + strlen("Voxino\\src\\")                  \
                    : __FILE__))


    #define MINITRACE_COLLECT_FUNCTION()                                                           \
        auto __minitrace_macros_location = std::source_location::current();                        \
        MTR_SCOPE(__MINITRACE_FILENAME__, __minitrace_macros_location.function_name())

    #define MINITRACE_COLLECT_FUNCTION_CUSTOM(custom_name)                                         \
        MTR_SCOPE(__MINITRACE_FILENAME__, custom_name)

#else
    #define MINITRACE_COLLECT_FUNCTION()
    #define MINITRACE_COLLECT_FUNCTION(custom_name)
#endif