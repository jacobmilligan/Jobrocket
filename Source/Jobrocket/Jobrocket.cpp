//
//  Jobrocket.cpp
//  Jobrocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 21/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include "Jobrocket/Jobrocket.hpp"

namespace jobrocket {

struct CurrentJobrocket {
    static constexpr uint32_t pool_capacity = 4096;

    static Scheduler scheduler;
    static JobPool job_pool;
};

Scheduler CurrentJobrocket::scheduler;
JobPool CurrentJobrocket::job_pool;

void startup(int32_t num_workers, int32_t num_main_threads)
{
    CurrentJobrocket::scheduler.startup(num_workers, num_main_threads);
    auto num_threads = CurrentJobrocket::scheduler.num_workers() + CurrentJobrocket::scheduler.num_main_threads();
    CurrentJobrocket::job_pool = JobPool(num_threads, CurrentJobrocket::pool_capacity);
}

void shutdown()
{
    CurrentJobrocket::scheduler.shutdown();
    CurrentJobrocket::job_pool.reset();
}

JobPool* current_job_pool()
{
    return &CurrentJobrocket::job_pool;
}

Scheduler* current_scheduler()
{
    return &CurrentJobrocket::scheduler;
}

void run(Job* job)
{
    CurrentJobrocket::scheduler.schedule_job(job);
}

void wait(const Job* job)
{
    while ( job->state != Job::State::completed ) {
        CurrentJobrocket::scheduler.thread_local_worker()->try_run_job();
    }
}

void register_main_thread()
{
    CurrentJobrocket::scheduler.register_main_thread();
}


} // namespace jobrocket