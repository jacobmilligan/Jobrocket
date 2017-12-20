//
//  JobPool.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 15/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "JobRocket/Job.hpp"

#include <cstdint>

namespace sky {


class JobPool {
public:
    JobPool(const uint32_t num_threads, const uint32_t capacity)
    {
        num_threads_ = num_threads;
        allocators_.resize(num_threads_);
        free_lists_.resize(num_threads_);

        for ( auto& fl : free_lists_) {
            fl = FreeList(capacity);
        }

        for ( auto& a : allocators_ ) {
            a = std::move(FixedPoolAllocator(sizeof(Job), capacity));
        }
    }

    template <typename Fn, typename... Args>
    Job* allocate_job(Fn function, Args&&... args)
    {
        auto& index = get_index();
        if ( free_lists_[index].full() ) {
            free_all_this_thread();
        }

        void* mem = allocators_[index].allocate();
        auto fn =  JobFunction<Fn, Args...>(function, std::forward<Args>(args)...);

        new (mem) Job(fn.size(), &fn, index);

        return static_cast<Job*>(mem);
    }

    void free_all_this_thread()
    {
        auto& index = get_index();
        while ( !free_lists_[index].empty() ) {
            allocators_[index].free(free_lists_[index].pop());
        }
    }

    void free_job(Job* job)
    {
        free_lists_[job->worker_alloc].push(job);
    }

    void reset()
    {
        for ( int i = 0; i < num_threads_; ++i ) {
            free_lists_[0].reset();
            allocators_[0].reset();
        }
    }
private:
    class FreeList {
    public:
        FreeList() = default;

        explicit FreeList(const size_t capacity)
            : next_(0), capacity_(capacity)
        {
            elements.resize(capacity_, nullptr);
        }

        void reset()
        {
            next_ = 0;
            for ( auto& e : elements ) {
                e = nullptr;
            }

            std::atomic_thread_fence(std::memory_order_seq_cst);
        }

        void push(Job* job)
        {
            auto n = next_++;
            elements[n] = job;
        }

        Job* pop()
        {
            if ( next_ == 0 ) {
                return nullptr;
            }
            auto n = --next_;

            auto* job = elements[n];
            return job;
        }

        inline bool empty() const
        {
            return next_ <= 0;
        }

        inline bool full() const
        {
            return next_ >= capacity_;
        }
    private:
        size_t next_{0};
        size_t capacity_;
        std::vector<Job*> elements;
    };

    uint32_t num_threads_;
    std::vector<FixedPoolAllocator> allocators_;
    std::vector<FreeList> free_lists_;

    uint32_t next_index{0};

    const uint32_t& get_index()
    {
        static thread_local uint32_t next = next_index++;
        return next;
    }
};


}



