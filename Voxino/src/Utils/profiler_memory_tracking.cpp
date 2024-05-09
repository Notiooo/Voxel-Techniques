#include "profiler_memory_tracking.h"
#include "pch.h"

#ifdef TRACY_AUTOMATIC_MEMORY_PROFILING

    #include <Tracy.hpp>
    #include <cstdlib>


void* operator new(size_t size)
{
    void* ptr = std::malloc(size);
    if (!ptr)
    {
        throw std::bad_alloc();
    }
    TracyAlloc(ptr, size);
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    TracyFree(ptr);
    std::free(ptr);
}

void* operator new[](size_t size)
{
    void* ptr = std::malloc(size);
    if (!ptr)
    {
        throw std::bad_alloc();
    }
    TracyAlloc(ptr, size);
    return ptr;
}

void operator delete[](void* ptr) noexcept
{
    TracyFree(ptr);
    std::free(ptr);
}

#endif