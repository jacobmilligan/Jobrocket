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

#include <hwloc.h>

namespace jobrocket {


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

void Scheduler::startup(const int32_t num_threads)
{
    if ( num_threads <= auto_worker_count ) {
        num_workers_ = auto_worker_count_value();
    } else {
        num_workers_ = static_cast<uint32_t>(num_threads);
    }

    workers_.resize(num_workers_);
    uint32_t worker_id = 0;
    for ( auto& w : workers_ ) {
        w = std::move(jobrocket::Worker(worker_id++, workers_.data(), num_workers_, job_capacity_per_worker));
    }

    // Start all worker threads except for worker 0. Worker 0 is reserved for the main thread so
    // needs no worker thread
    for ( auto& w : workers_ ) {
        if ( w.id() > 0 ) {
            w.start();
        }
    }
}

void Scheduler::shutdown()
{
    for ( auto& w : workers_ ) {
        w.stop();
    }
}

Worker* Scheduler::find_local_worker()
{
    for ( int w = 0; w < workers_.size(); ++w ) {
        if ( workers_[w].owns_this_thread() ) {
            return &workers_[w];
        }
    }

    // Main thread
    return &workers_[0];
}

Worker* Scheduler::thread_local_worker()
{
    static thread_local Worker* local_worker = find_local_worker();

    return local_worker;
}

void Scheduler::schedule_job(Job* job)
{
    thread_local_worker()->schedule_job(job);
}


} // namespace jobrocket