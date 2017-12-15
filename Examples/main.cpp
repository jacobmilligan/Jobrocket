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
    if ( iteration >= num_jobs ) {
        active = false;
    }
}

int main(int argc, char** argv)
{

    sched.startup(sky::Scheduler::auto_worker_count, sky::Scheduler::default_worker_job_capacity);

    sky::Job job;
//    for ( int i = 0; i < num_jobs; ++i ) {
//        job = sky::make_job(thread_proc, num_jobs);
//        sched.run_job(job);
//    }

    auto start = std::chrono::high_resolution_clock::now();

    for ( int i = 0; i <= num_jobs; ++i ) {
        job = sky::make_job(long_task, static_cast<uint32_t>(i));
        sched.run_job(job);
//        long_task(i);
    }
    while (active) {
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto millis = std::chrono::duration<double, std::milli>(end - start);

    printf("Time: %lf\n", millis.count());

    printf("Expected: %d. Total: %d\n", expected, total);

    return 0;
}