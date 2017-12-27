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

namespace jobrocket {


/// A `Scheduler` takes pre-allocated jobs and schedules them onto worker threads owned by the
/// internal scheduler
class Scheduler {
public:
    /// Flag for letting the scheduler decide how many worker threads to create
    static constexpr int32_t auto_worker_count = -1;
    static constexpr uint32_t job_capacity_per_worker = 4096;

    Scheduler() = default;

    /// Calls shutdown before destruction
    ~Scheduler();

    /// Gets the number of threads created when `auto_worker_count` is used on startup
    uint32_t auto_worker_count_value();

    /// Starts the scheduler and all worker threads. Makes the startup thread wait until all workers
    /// are ready
    void startup(int32_t num_threads);

    /// Shuts down the scheduler by waiting until all workers have completed the current job. All
    /// jobs sitting in worker queues are discarded
    void shutdown();

    /// Schedules a job onto the current threads local worker queue, making the job ready to
    /// execute
    void schedule_job(Job* job);

    /// Gets a pointer to the current threads associated worker
    Worker* thread_local_worker();

    /// Gets the number of real CPU cores on the current platform
    inline uint32_t core_count()
    {
        return num_cores_;
    }

    /// Gets the number of threads available on the current platform including hyperthreads
    inline uint32_t hardware_thread_count()
    {
        return num_hw_threads_;
    }

    /// Gets the number of workers active within the scheduler
    inline uint32_t num_workers()
    {
        return num_workers_;
    }

private:
    uint32_t num_workers_{0};
    uint32_t num_cores_{0};
    uint32_t num_hw_threads_{0};

    std::vector<Worker> workers_;

    Worker* find_local_worker();
};


} // namespace jobrocket