#pragma once

#include <BasicGlobal.h>
#include <vector>

namespace bsc
{
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