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
    JobPool() = default;

    static void init(const uint32_t capacity)
    {
        allocator_.init(sizeof(Job), capacity);
    }

    template <typename Fn, typename... Args>
    static Job* allocate_job(Fn function, Args&&... args)
    {
        auto* mem = allocator_.allocate();
        new (mem) JobFunction<Fn, Args...>(function, std::forward<Args>(args)...);
        return static_cast<Job*>(mem);
    }

    static void free_job(Job* job)
    {
        allocator_.free(job);
    }
private:
    static thread_local StaticPoolAllocator allocator_;
};

thread_local StaticPoolAllocator JobPool::allocator_;


}



