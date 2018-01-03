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


class JobGroup {
public:
    JobGroup()
        : job_count_(0)
    {}

    template <typename Fn, typename... Args>
    void run(Fn function, Args&&... args)
    {
        job_count_.increment();

        auto job = jobrocket::make_job(function, std::forward<Args>(args)...);
        job->group_counter = &job_count_;

        jobrocket::run(job);
    }

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