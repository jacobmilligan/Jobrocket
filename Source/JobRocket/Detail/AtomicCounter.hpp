//
//  AtomicCounter.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 15/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include <cstdint>
#include <atomic>

namespace jobrocket {
namespace detail {


class AtomicCounter {
public:
    explicit AtomicCounter(uint32_t initial_val = 0)
    {
        counter_.store(initial_val, std::memory_order_relaxed);
    }

    uint32_t load()
    {
        return counter_.load(std::memory_order_relaxed);
    }

    void store(uint32_t value)
    {
        counter_.store(value, std::memory_order_relaxed);
    }

    void decrement()
    {
        auto val = counter_.load(std::memory_order_acquire);
        if ( val > 0 ) {
            counter_.store(val - 1, std::memory_order_release);
        }
    }

    void increment()
    {
        auto val = counter_.load(std::memory_order_acquire);
        if ( val < UINT32_MAX ) {
            counter_.store(val + 1, std::memory_order_release);
        }
    }

private:
    std::atomic_uint counter_{0};
};


} // namespace detail
} // namespace jobrocket