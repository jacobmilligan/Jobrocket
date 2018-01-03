//
//  MinimalExample.cpp
//  JobRocket Examples
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 9/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include <JobRocket/JobRocket.hpp>
#include <JobRocket/JobGroup.hpp>

int main()
{
    // Launch the scheduler with the desired number of workers and main-threads.
    // This example leaves the number of workers and main threads up to the scheduler which will
    // usually be the physical cores + 1 workers and 1 main thread
    jobrocket::startup(jobrocket::Scheduler::auto_thread_count,
                       jobrocket::Scheduler::auto_thread_count);

    // Allocate a few jobs using the job pool but don't run them
    auto* job1 = jobrocket::make_job([]() {
        printf("Hello from job 1\n");
    });

    auto* job2 = jobrocket::make_job([]() {
        printf("Hello from job 2\n");
    });

    // Allocate a job, run it, and then wait for the result
    jobrocket::make_job_and_wait([]() {
        printf("Hello from job 3\n");
    });

    // Run the earlier-allocated jobs
    jobrocket::run(job1);
    jobrocket::run(job2);

    // Wait for both to finish
    jobrocket::wait(job2);
    jobrocket::wait(job1);

    // JobRocket provides the `JobGroup` class to run many related jobs in parallel and wait on
    // all of them to finish
    jobrocket::JobGroup group;

    constexpr int num_jobs = 200;
    constexpr int expected_result = (num_jobs * (num_jobs + 1)) / 2;
    int value = 0;

    printf("\nJobGroup: Sum of 1..%d:\n", num_jobs);

    for ( int i = 1; i <= num_jobs; ++i ) {
        group.run([&, i]() {
            value += i;
        });
    }

    // Wait for all jobs to finish
    group.wait_for_all();


    printf("Expected value: %d | Final value: %d\n", expected_result, value);

    jobrocket::shutdown();
}