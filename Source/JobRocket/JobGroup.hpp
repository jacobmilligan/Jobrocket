//
//  JobGroup.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 14/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include <atomic>
#include "JobRocket/AtomicCounter.hpp"
#include "JobRocket/Scheduler.hpp"

namespace sky {


class JobGroup {
public:
    explicit JobGroup(Scheduler* scheduler)
        : job_count_(0), scheduler_(scheduler)
    {}

    template <typename Fn, typename... Args>
    void run(Fn function, Args&&... args)
    {
        job_count_.increment();

        auto job = make_job(function, std::forward<Args...>(args)...);
        job.group_counter = &job_count_;

        scheduler_->run_job(job);
    }

    void wait_for_all()
    {
        Job* next_job = nullptr;
        while ( job_count_.load() > 0 ) {
            next_job = scheduler_->thread_local_worker()->get_next_job();
            if ( next_job != nullptr ) {
                next_job->execute();
            }
        }
    }
private:
    AtomicCounter job_count_;
    Scheduler* scheduler_;
};


}