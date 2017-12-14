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

namespace sky {

thread_local Worker* Scheduler::this_thread_worker_ = nullptr;

Scheduler::~Scheduler()
{
    shutdown();
}

u32 Scheduler::default_thread_count()
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

void Scheduler::startup(const i32 num_threads, const u32 worker_job_capacity)
{
    if ( num_threads <= auto_worker_count ) {
        num_workers_ = default_thread_count();
    } else {
        num_workers_ = static_cast<u32>(num_threads);
    }

    workers_.resize(num_workers_);
    u32 worker_id = 0;
    for ( auto& w : workers_ ) {
        w = std::move(sky::Worker(worker_id++, workers_.data(), num_workers_, worker_job_capacity));
    }

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

void Scheduler::run_job(Job& job)
{
    if ( this_thread_worker_ == nullptr ) {
        for ( int w = 0; w < workers_.size(); ++w ) {
            if ( workers_[w].owns_this_thread() ) {
                this_thread_worker_ = &workers_[w];
                break;
            }
        }

        if ( this_thread_worker_ == nullptr ) {
            this_thread_worker_ = &workers_[0];
        }
    }

    this_thread_worker_->schedule_job(job);
}


}