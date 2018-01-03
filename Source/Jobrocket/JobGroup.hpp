//
//  JobGroup.hpp
//  Jobrocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 14/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "Jobrocket/Detail/AtomicCounter.hpp"
#include "Jobrocket/Jobrocket.hpp"

#include <atomic>

namespace jobrocket {


/// A grouping of many jobs that can be waited on until the final job in the group finishes
/// execution
class JobGroup {
public:
    JobGroup()
        : job_count_(0)
    {}

    /// Adds a job to the group and runs it
    template <typename Fn, typename... Args>
    void run(Fn function, Args&&... args)
    {
        job_count_.increment();

        auto job = jobrocket::make_job(function, std::forward<Args>(args)...);
        job->group_counter = &job_count_;

        jobrocket::run(job);
    }

    /// Waits for all jobs in the group to finish execution. Rather than sleeping or busy-waiting
    /// this function will cause the calling thread to start popping and stealing jobs from queues
    /// in an effort to help with the schedulers work-load
    void wait_for_all()
    {
        while ( job_count_.load() > 0 ) {
            current_scheduler()->thread_local_worker()->try_run_job();
        }
    }
private:
    detail::AtomicCounter job_count_;
};


} // namespace jobrocket