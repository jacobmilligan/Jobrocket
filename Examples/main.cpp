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
#include <JobRocket/JobGroup.hpp>

sky::Scheduler sched;

uint32_t total = 0;
static constexpr uint32_t num_jobs = 12;
static constexpr uint32_t expected = (num_jobs * (num_jobs + 1)) / 2;

uint32_t job_results[num_jobs];

bool active = true;

void long_task(uint32_t iteration)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    total += iteration;
}

int main(int argc, char** argv)
{

    sched.startup(sky::Scheduler::auto_worker_count, sky::Scheduler::default_worker_job_capacity);

    sky::JobGroup group(&sched);

    for ( int i = 0; i <= num_jobs; ++i ) {
        group.run(long_task, static_cast<uint32_t>(i));
    }

    group.wait_for_all();

    printf("Total: %d. Expected: %d\n", total, expected);

    return 0;
}