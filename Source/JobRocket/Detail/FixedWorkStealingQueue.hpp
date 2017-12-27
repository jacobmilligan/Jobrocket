//
//  FixedWorkStealingQueue.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 9/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "JobRocket/Job.hpp"

#include <atomic>
#include <vector>

//
//  Chase, D & Lev, Y 2005, 'Dynamic Circular Work-stealing Deque',
//  Proceedings of the Seventeenth Annual ACM Symposium on Parallelism
//  in Algorithms and Architectures, ACM, Las Vegas, Nevada, USA,
//  pp. 21–28, viewed <http://doi.acm.org/10.1145/1073970.1073974>.
//

namespace jobrocket {

class FixedWorkStealingQueue {
public:
    FixedWorkStealingQueue() = default;

    /// @brief Initializes the queue with `capacity` max number of jobs
    /// @param capacity
    explicit FixedWorkStealingQueue(const size_t capacity)
        : top_(0), bottom_(0), capacity_(capacity)
    {
        jobs_.resize(capacity);
    }

    FixedWorkStealingQueue(const FixedWorkStealingQueue& other) = delete;
    FixedWorkStealingQueue& operator=(const FixedWorkStealingQueue& other) = delete;

    FixedWorkStealingQueue(FixedWorkStealingQueue&& other) noexcept
        : jobs_(std::move(other.jobs_)),
          capacity_(other.capacity_),
          bottom_(other.bottom_.load()),
          top_(other.top_.load())
    {}

    FixedWorkStealingQueue& operator=(FixedWorkStealingQueue&& other) noexcept
    {
        jobs_ = std::move(other.jobs_);
        capacity_ = other.capacity_;
        bottom_ = other.bottom_.load();
        top_ = other.top_.load();

        return *this;
    }

    /// @brief Pushes a new job onto the bottom of the queue - must be called from owning thread
    /// @param job
    /// @return True if successful push, false otherwise
    bool push(Job* job)
    {
        auto b = bottom_.load(std::memory_order_relaxed);
        auto t = top_.load(std::memory_order_acquire);

        jobs_[b % capacity_] = job;

        std::atomic_thread_fence(std::memory_order_release);
        bottom_.store(b + 1, std::memory_order_relaxed);
        return true;
    }

    /// @brief Pops the next job from the bottom of the queue - must be called from owning thread
    /// @param target
    /// @return True if popped successfully, false otherwise
    bool pop(Job*& target)
    {
        // Ensure bottom is decremented first to avoid popping duplicates
        auto b = --bottom_;
        std::atomic_thread_fence(std::memory_order_seq_cst);

        auto t = top_.load(std::memory_order_relaxed);

        bool result = false;

        // Check for non-empty queue
        if ( t <= b ) {
            result = true;
            target = jobs_[b % capacity_];
            // Check if this is the last element in queue
            if ( t == b ) {
                // Return false if race lost with stealing thread
                if ( !top_.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed) ) {
                    result = false;
                }

                // last element - don't store decremented value
                bottom_.store(b + 1, std::memory_order_relaxed);
            }
        } else {
            // Empty queue - don't store decremented value
            bottom_.store(b + 1, std::memory_order_relaxed);
        }

        return result;
    }

    /// @brief Steals a job from the top of the queue - can be called from any thread
    /// @param target
    /// @return True if successful steal, false otherwise
    bool steal(Job*& target)
    {
        // Ensure top is read first
        auto t = top_.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);

        auto b = bottom_.load(std::memory_order_acquire);
        bool result = false;

        // Return false if queue is empty
        if ( b - t <= 0 ) {
            return result;
        }

        // Check if queue is non-empty
        if ( t < b ) {
            result = true;
            target = jobs_[t % capacity_];

            // Return false if race lost with popping or stealing thread
            if ( !top_.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed) ) {
                result = false;
            }
        }

        return result;

    }

    /// @brief Checks if the queue is empty - note: not guaranteed to be accurate in concurrent
    /// environments for all threads
    /// @return True if empty, false otherwise
    inline bool empty() const noexcept
    {
        auto b = bottom_.load(std::memory_order_relaxed);
        auto t = top_.load(std::memory_order_relaxed);
        return b <= t;
    }
private:
    size_t capacity_{0};
    std::atomic<uint64_t> top_{0};
    std::atomic<uint64_t> bottom_{0};

    std::vector<Job*> jobs_;
};


}