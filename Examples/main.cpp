//
//  main.cpp
//  Tasks
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 12/11/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include <JobRocket/Scheduler.hpp>

#include <vector>
#include <thread>

sky::u32 total = 0;

void thread_proc(sky::u32 num)
{
    total += num;
}

int main(int argc, char** argv)
{
    sky::Scheduler sched;

    sched.startup(sky::Scheduler::auto_worker_count, sky::Scheduler::default_worker_job_capacity);

    for ( int i = 0; i < 200; ++i ) {
        auto job = sky::make_job(thread_proc, static_cast<sky::u32>(i));
        sched.run_job(job);
    }

    return 0;
}