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

#include <iostream>

int main()
{
    jobrocket::startup();

    auto* job1 = jobrocket::make_job([]() {
        std::cout << "Hello from job 1\n";
    });

    auto* job2 = jobrocket::make_job([]() {
        std::cout << "Hello from job 2\n";
    });

    jobrocket::make_job_and_wait([]() {
        std::cout << "Hello from job 3\n";
    });

    jobrocket::run(job1);
    jobrocket::run(job2);

    jobrocket::wait(job1);
    jobrocket::wait(job2);

//    jobrocket::JobGroup group;
//
//    for ( int i = 0; i < 10; ++i ) {
//        group.run([i]() {
//            std::cout << "Hello from group job " << i << "\n";
//        });
//    }
//
//    group.wait_for_all();

    jobrocket::shutdown();
}