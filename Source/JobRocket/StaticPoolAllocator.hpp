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

#include <cstdint>
#include <cstddef>

namespace sky {


class StaticPoolAllocator {
public:
    StaticPoolAllocator() = default;

    ~StaticPoolAllocator()
    {
        destroy();
    }

    // TODO(Jacob): Implement alignment
    void init(const size_t block_size, const size_t max_blocks)
    {
        block_size_ = block_size;
        max_blocks_ = max_blocks;

        memory_ = new uint8_t[block_size_ * max_blocks_];

        reset();
    }

    void destroy()
    {
        delete[] memory_;
    }

    /// @brief Allocates a new block of memory from the pool
    /// @return Pointer to the memory
    void* allocate()
    {
        void* block = next_;

        if ( blocks_initialized_ < max_blocks_ ) {
            ++blocks_initialized_;

            if ( blocks_initialized_ < max_blocks_ ) {
                next_ = memory_ + (blocks_initialized_ * block_size_);
            } else {
                next_ = nullptr;
            }
        }

        return block;
    }

    /// @brief Frees memory allocated by allocate(). Provides undefined behavior if the data
    /// wasn't allocated with allocate() and doesn't belong to memory_
    /// @param data Data to free
    void free(void* data)
    {
        next_ = static_cast<uint8_t*>(data);
        --blocks_initialized_;
    }

    void reset()
    {
        next_ = memory_;
        blocks_initialized_ = 0;
    }
private:
    uint8_t* memory_{nullptr};
    uint8_t* next_{nullptr};

    size_t block_size_{0};
    size_t max_blocks_{0};

    uint32_t blocks_initialized_{0};
};


}