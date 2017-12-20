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

#include "JobRocket/Worker.hpp"

namespace sky {


class Scheduler {
public:
    static constexpr int32_t auto_worker_count = -1;
    static constexpr uint32_t job_capacity_per_worker = 4096;

    Scheduler() = default;

    ~Scheduler();

    uint32_t default_thread_count();

    void startup(int32_t num_threads);

    void shutdown();

    void run_job(Job* job);

    void wait(Job& job);

    Worker* thread_local_worker();

    inline uint32_t core_count()
    {
        return num_cores_;
    }

    inline uint32_t hardware_thread_count()
    {
        return num_hw_threads_;
    }

private:
    uint32_t num_workers_{0};
    uint32_t num_cores_{0};
    uint32_t num_hw_threads_{0};

    std::vector<Worker> workers_;

    Worker* find_local_worker();
};


}