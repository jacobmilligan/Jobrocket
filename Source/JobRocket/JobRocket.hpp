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


void startup(int32_t num_threads = Scheduler::auto_worker_count);

void shutdown();

JobPool* current_job_pool();

Scheduler* current_scheduler();

template <typename Fn, typename... Args>
static Job* make_job(Fn function, Args&& ... args)
{
    return current_job_pool()->allocate_job(function, std::forward<Args>(args)...);
};

void run(Job* job);

void wait(const Job* job);

template <typename Fn, typename... Args>
static void make_job_and_wait(Fn function, Args&& ... args)
{
    auto* job = make_job(function, std::forward<Args>(args)...);
    current_scheduler()->run_job(job);
    wait(job);
};


} // namespace jobrocket
