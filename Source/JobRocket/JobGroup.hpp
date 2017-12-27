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
#include "JobRocket/Detail/AtomicCounter.hpp"
#include "JobRocket/JobRocket.hpp"

namespace jobrocket {


class JobGroup {
public:
    JobGroup()
        : job_count_(0)
    {}

    template <typename Fn, typename... Args>
    void run(Fn function, Args&&... args)
    {
        job_count_.increment();

        auto job = make_job(function, std::forward<Args>(args)...);
        job->group_counter = &job_count_;

        current_scheduler()->run_job(job);
    }

    void wait_for_all()
    {
        Job* next_job = nullptr;
        while ( job_count_.load() > 0 ) {
            next_job = current_scheduler()->thread_local_worker()->get_next_job();
            if ( next_job != nullptr ) {
                next_job->execute();
            }
        }
    }
private:
    AtomicCounter job_count_;
};


}