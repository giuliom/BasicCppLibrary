#pragma once

#include <BasicGlobal.h>
#include <functional>

namespace bsc
{

    template <class Key, class Value, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
    class BasicCache {
    public:
        explicit BasicCache(std::size_t capacity) : capacity_(capacity) {
            if (capacity_ == 0) {
                throw std::invalid_argument("BasicCache capacity must be > 0");
            }
        }

        bool get(const Key& key, Value& value) const {
            //TODO: Implement the get method
            return true; // Placeholder return value
        }

        bool put(const Key& key, const Value& value) {
            // TODO: Implement the put method
            return true; // Placeholder return value
        }

    private:
        std::size_t capacity_;
    };

} // namespace bsc