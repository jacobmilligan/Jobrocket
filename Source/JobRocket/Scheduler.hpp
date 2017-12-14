//
//  Scheduler.hpp
//  Tasks
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 11/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "JobRocket/NumericTypes.hpp"
#include "JobRocket/Worker.hpp"

namespace sky {


class Scheduler {
public:
    static constexpr i32 auto_worker_count = -1;
    static constexpr u32 default_worker_job_capacity = 4096;

    Scheduler() = default;

    ~Scheduler();

    u32 default_thread_count();

    void startup(i32 num_threads, u32 worker_job_capacity);

    void shutdown();

    void run_job(Job& job);

    inline u32 core_count()
    {
        return num_cores_;
    }

    inline u32 hardware_thread_count()
    {
        return num_hw_threads_;
    }
private:
    u32 num_workers_{0};
    u32 num_cores_{0};
    u32 num_hw_threads_{0};

    std::vector<Worker> workers_;

    static thread_local Worker* this_thread_worker_;
};


}