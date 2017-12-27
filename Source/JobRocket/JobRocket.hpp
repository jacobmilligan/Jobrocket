//
//  JobRocket.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 21/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once


#include "JobRocket/JobPool.hpp"
#include "JobRocket/Scheduler.hpp"

#include <cstdint>

namespace jobrocket {

/// Starts up the global `Scheduler` and `JobPool`. If no value is specified for `num_workers` the
/// scheduler decides how workers to create based off the current hardware - in most circumstances
/// this is the most appropriate value.
void startup(int32_t num_workers = Scheduler::auto_worker_count);

/// Shuts down the global scheduler by waiting until all workers have completed the current job.
/// All jobs sitting in worker queues are discarded
void shutdown();

/// Gets a pointer to the currently active global `JobPool`
JobPool* current_job_pool();

/// Gets a pointer to the currently active global `Scheduler`
Scheduler* current_scheduler();

/// Allocates a new job from the global pool with the job function and arguments given
template <typename Fn, typename... Args>
Job* make_job(Fn function, Args&& ... args)
{
    return current_job_pool()->allocate_job(function, std::forward<Args>(args)...);
}

/// Takes a pre-allocated `Job` and schedules it to the currently active global scheduler
void run(Job* job);

/// Takes a pre-allocated and pre-scheduled `Job` and waits until it has completed.
/// While waiting, the current thread will attempt to assist with completing scheduled jobs rather
/// than going to sleep or busy waiting
void wait(const Job* job);

/// Allocates a new `Job` using the current global pool, schedules it and then makes the
/// current thread wait until the jobs completion. This is analogous to calling:
///
/// ```
/// auto job = jobrocket::make_job(example_job_function);
/// jobrocket::run(job);
/// jobrocket::wait(job);
/// ```
template <typename Fn, typename... Args>
void make_job_and_wait(Fn function, Args&& ... args)
{
    auto* job = make_job(function, std::forward<Args>(args)...);
    current_scheduler()->schedule_job(job);
    wait(job);
}


} // namespace jobrocket
