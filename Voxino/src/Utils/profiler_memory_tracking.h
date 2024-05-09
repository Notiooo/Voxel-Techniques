#ifdef TRACY_AUTOMATIC_MEMORY_PROFILING

void* operator new(size_t size);

void operator delete(void* ptr) noexcept;

void* operator new[](size_t size);

void operator delete[](void* ptr) noexcept;

#endif