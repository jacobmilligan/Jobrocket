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


#include <cstdint>
#include "JobRocket/JobPool.hpp"

namespace jobrocket {

class Scheduler;

void startup(int32_t num_threads = -1);

void shutdown();

JobPool* current_job_pool();

Scheduler* current_scheduler();

template <typename Fn, typename... Args>
static Job* make_job(Fn function, Args&& ... args)
{
    return current_job_pool()->allocate_job(function, std::forward<Args>(args)...);
};




}
