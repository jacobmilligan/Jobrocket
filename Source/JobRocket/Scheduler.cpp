//
//  Scheduler.cpp
//  Tasks
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 11/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include "JobRocket/Scheduler.hpp"
#include "JobRocket/Detail/Error.hpp"

#include <hwloc.h>

namespace jobrocket {

constexpr uint32_t Scheduler::job_capacity_per_worker;
constexpr int32_t Scheduler::auto_thread_count;


Scheduler::~Scheduler()
{
    shutdown();
}

uint32_t Scheduler::auto_worker_count_value()
{
    if ( num_cores_ == 0 || num_hw_threads_ == 0 ) {
        hwloc_topology_t topology;
        hwloc_topology_init(&topology);
        hwloc_topology_load(topology);

        if ( num_cores_ == 0 ) {
            num_cores_ = static_cast<uint32_t>(hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_CORE));
        }
        if ( num_hw_threads_ == 0 ) {
            num_hw_threads_ = static_cast<uint32_t>(hwloc_get_nbobjs_by_type(topology,
                                                                             HWLOC_OBJ_PU));
        }
    }

    return num_cores_;
}

void Scheduler::startup(const int32_t num_threads, const int32_t num_main_threads)
{
    active_jobs_.store(0);

    if ( num_threads <= auto_thread_count ) {
        num_workers_ = auto_worker_count_value();
    } else {
        num_workers_ = static_cast<uint32_t>(num_threads);
    }

    if ( num_main_threads > auto_thread_count ) {
        num_main_threads_ = static_cast<uint32_t>(num_main_threads);
    }

    workers_.resize(num_workers_ + num_main_threads + 1);
    uint32_t worker_id = 0;
    for ( auto& w : workers_ ) {
        w = std::move(Worker(worker_id++, &workers_, job_capacity_per_worker, &worker_mut_, &worker_cv_));
    }

    // Start all worker threads except main threads
    for ( int i = 0; i < num_workers_; ++i ) {
        workers_[i].start();
    }

    next_main_ = num_workers_;
}

void Scheduler::shutdown()
{
    for ( auto& w : workers_ ) {
        w.terminate();
    }
    worker_cv_.notify_all();
    for ( auto& w : workers_ ) {
        w.join();
    }
}

void Scheduler::register_main_thread()
{
    // Locking to avoid races with workers trying to call `thread_local_worker`
    std::unique_lock<std::mutex> lock(main_thread_mut_);

    auto result = main_thread_map_.find(std::this_thread::get_id());

    if ( result != main_thread_map_.end() ) {
        detail::print_error("Scheduler", "Attempted to register a new main thread that had already "
            "been registered");
        return;
    }

    auto index = next_main_++;

    if ( index >= num_workers_ + num_main_threads_ ) {
        detail::print_error("Scheduler", "Attempted to add more main threads than were "
            "specified at startup", "Try registering all main threads at application start "
            "rather than ad-hoc");
        return;
    }

    main_thread_map_[std::this_thread::get_id()] = index;
}

Worker* Scheduler::find_local_worker()
{
    // Locking here is okay for performance and deadlock concerns as it should only be called
    // once per thread per scheduler
    std::unique_lock<std::mutex> lock(main_thread_mut_);

    // Check if this thread is a regular worker thread
    for ( int w = 0; w < num_workers_; ++w ) {
        if ( workers_[w].owns_this_thread() ) {
            return &workers_[w];
        }
    }

    // Check if previously-registered main thread
    auto result = main_thread_map_.find(std::this_thread::get_id());
    if ( result != main_thread_map_.end() ) {
        return &workers_[result->second];
    }

    // Couldn't find a registered main thread so this must be either
    // the programs main() thread or some unregistered thread in which case we get undefined
    // behavior which is the intended consequence of scheduling a job on a thread that the
    // scheduler doesn't know about
    return &workers_.back();
}

Worker* Scheduler::thread_local_worker()
{
    static thread_local Worker* local_worker = find_local_worker();
    return local_worker;
}

void Scheduler::schedule_job(Job* job)
{
    thread_local_worker()->schedule_job(job);
    worker_cv_.notify_all();
}


} // namespace jobrocket