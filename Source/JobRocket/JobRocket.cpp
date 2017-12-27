//
//  JobRocket.cpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 21/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include "JobRocket/JobRocket.hpp"
#include "JobRocket/Scheduler.hpp"

namespace jobrocket {

struct JobRocket {
    static constexpr uint32_t pool_capacity = 4096;

    static Scheduler scheduler;
    static JobPool job_pool;
};

Scheduler JobRocket::scheduler;
JobPool JobRocket::job_pool;

void startup(int32_t num_threads)
{
    JobRocket::scheduler.startup(num_threads);
    JobRocket::job_pool = JobPool(JobRocket::scheduler.num_workers(), JobRocket::pool_capacity);
}

void shutdown()
{
    JobRocket::scheduler.shutdown();
    JobRocket::job_pool.reset();
}

JobPool* current_job_pool()
{
    return &JobRocket::job_pool;
}

Scheduler* current_scheduler()
{
    return &JobRocket::scheduler;
}


}