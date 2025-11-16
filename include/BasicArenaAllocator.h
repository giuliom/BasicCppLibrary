#pragma once

#include <BasicGlobal.h>
#include <vector>

namespace bsc
{
    // Allow de-allocation and keep searcheable available spaces for re-allocation. spaces should be merged

    // Use memory pool style for block sizes

    // Use custom virtual memory address type to allow resizing of the arena/paging

    // https://en.wikipedia.org/wiki/Buddy_memory_allocation

    template <typename T>
    class BasicArenaAllocator {
    public:
        using value_type = T;

        BasicArenaAllocator() = default;

        template <typename U>
        BasicArenaAllocator(const BasicArenaAllocator<U>&) {}

        T* allocate(std::size_t n) {
            // TODO: Implement memory allocation from the arena
            return nullptr;
        }

        void deallocate(T* p, std::size_t) {
            // TODO: Implement memory deallocation back to the arena
        }
    };

} // namespace bsc