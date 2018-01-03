//
//  PoolAllocator.hpp
//  Tasks
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 28/11/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "Jobrocket/Detail/Error.hpp"

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <algorithm>

namespace jobrocket {
namespace detail {


// TODO(Jacob): Implement alignment
class FixedPoolAllocator {
public:
    FixedPoolAllocator() = default;

    FixedPoolAllocator(const size_t block_size, const uint32_t max_blocks)
    {
        assert(block_size >= sizeof(void*));

        block_size_ = block_size;
        max_blocks_ = max_blocks;
        memory_ = new uint8_t[block_size_ * max_blocks_];

        reset();
    }

    FixedPoolAllocator(const FixedPoolAllocator& other) = delete;
    FixedPoolAllocator& operator=(const FixedPoolAllocator& other) = delete;

    FixedPoolAllocator(FixedPoolAllocator&& other) noexcept
    {
        std::swap(memory_, other.memory_);
        std::swap(next_, other.next_);
        std::swap(block_size_, other.block_size_);
        std::swap(max_blocks_, other.max_blocks_);
        std::swap(blocks_initialized_, other.blocks_initialized_);
    }

    FixedPoolAllocator& operator=(FixedPoolAllocator&& other) noexcept
    {
        std::swap(memory_, other.memory_);
        std::swap(next_, other.next_);
        std::swap(block_size_, other.block_size_);
        std::swap(max_blocks_, other.max_blocks_);
        std::swap(blocks_initialized_, other.blocks_initialized_);

        return *this;
    }

    ~FixedPoolAllocator()
    {
        delete[] memory_;
    }

    /// @brief Allocates a new block of memory from the pool
    /// @return Pointer to the memory
    void* allocate()
    {
        if ( blocks_initialized_ == max_blocks_ ) {
            return nullptr;
        }

        ++blocks_initialized_;

        uint8_t* block = next_;
        next_ = reinterpret_cast<uint8_t**>(next_)[0];
        return block;
    }

    /// @brief Frees memory allocated by allocate(). Provides undefined behavior if the data
    /// wasn't allocated with allocate() and doesn't belong to memory_
    /// @param data Data to free
    bool free(void* data)
    {
        if ( data == nullptr ) {
            print_error("FixedPoolAllocator", "tried to free a null pointer");
            return false;
        }

        if ( blocks_initialized_ <= 0 ) {
            print_error("FixedPoolAllocator",
                        "tried to free more blocks than the pool has available",
                        "ensure all allocations/frees are in matching pairs");
            return false;
        }

        if ( data > &memory_[block_size_ * max_blocks_] || data < &memory_[0] ) {
            print_error("FixedPoolAllocator",
                        "attempted to free memory that wasn't allocated by the pool",
                        "consider tracking all allocations to ensure they come from the same pool");
            return false;
        }

        reinterpret_cast<uint8_t**>(data)[0] = next_;
        next_ = static_cast<uint8_t*>(data);
        --blocks_initialized_;
        return true;
    }

    void reset()
    {
        next_ = memory_;
        blocks_initialized_ = 0;

        uint8_t* ptr = &memory_[0];
        uint8_t* end = &memory_[block_size_ * max_blocks_];
        while ( ptr < end ) {
            reinterpret_cast<uint8_t**>(ptr)[0] = ptr + block_size_;

            ptr += block_size_;
        }
    }

    inline uint32_t block_capacity()
    {
        return max_blocks_;
    }

    inline uint32_t blocks_initialized()
    {
        return blocks_initialized_;
    }
private:
    uint8_t* memory_{nullptr};
    uint8_t* next_{nullptr};

    size_t block_size_{0};
    uint32_t max_blocks_{0};

    uint32_t blocks_initialized_{0};
};


}
}