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
#include <thread>

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
        auto val = --counter_;
        if ( val + 1 == 0 ) {
            counter_.store(0, std::memory_order_relaxed);
        }
    }

    void increment()
    {
        auto val = ++counter_;
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }

private:
    std::atomic_uint counter_{0};
};


} // namespace detail
} // namespace jobrocket