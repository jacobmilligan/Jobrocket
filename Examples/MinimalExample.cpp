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
    jobrocket::startup();

    auto* job1 = jobrocket::make_job([]() {
        printf("Hello from job 1\n");
    });

    auto* job2 = jobrocket::make_job([]() {
        printf("Hello from job 2\n");
    });

    jobrocket::make_job_and_wait([]() {
        printf("Hello from job 3\n");
    });

    jobrocket::run(job1);
    jobrocket::run(job2);

    jobrocket::wait(job2);
    jobrocket::wait(job1);

    jobrocket::JobGroup group;

    constexpr int num_jobs = 10;
    constexpr int expected_result = (num_jobs * (num_jobs + 1)) / 2;
    int value = 0;

    printf("\nJobGroup: Sum of 1..%d:\n", num_jobs);

    for ( int i = 1; i <= num_jobs; ++i ) {
        group.run([&, i]() {
            value += i;
        });
    }

    group.wait_for_all();

    printf("Expected value: %d | Final value: %d\n", expected_result, value);

    jobrocket::shutdown();
}