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

void run(Job* job)
{
    JobRocket::scheduler.run_job(job);
}

void wait(const Job* job)
{
    Job* next_job = nullptr;
    while ( job->state != Job::State::completed ) {
        next_job = JobRocket::scheduler.thread_local_worker()->get_next_job();
        if ( next_job != nullptr ) {
            next_job->execute();
        }
    }
}


}