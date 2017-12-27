//
//  TestJobPool.cpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 20/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include <JobRocket/JobPool.hpp>

#include "catch/catch.hpp"

void test_job_fn(uint32_t index)
{
    printf("Test %d\n", index);
}

TEST_CASE("Job pool hash is unique per thread", "[job_pool]")
{
    constexpr uint32_t num_threads = 8;

    std::thread threads[num_threads];
    uint32_t hashes[num_threads];

    jobrocket::JobPool pool(num_threads, 100);

    bool waiting = true;

    for ( int i = 0; i < num_threads; ++i ) {
        threads[i] = std::thread([&, i]() {
            while ( waiting ) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
            }

            auto* job = pool.allocate_job(test_job_fn, i);
            hashes[i] = job->worker_alloc;
            job->execute();
        });
    }

    waiting = false;

    for ( auto& t : threads ) {
        if ( t.joinable() ) {
            t.join();
        }
    }

    for ( int i = 0; i < num_threads; ++i ) {
        for ( int j = 0; j < num_threads; ++j ) {
            if ( i != j ) {
                REQUIRE(hashes[i] != hashes[j]);
            }
        }
    }
}

TEST_CASE("Freeing jobs works as intended", "[job_pool]")
{
    jobrocket::JobPool pool(2, 100);

    jobrocket::Job* j1 = nullptr;
    jobrocket::Job* j2 = nullptr;
    jobrocket::Job* j3 = nullptr;

    std::thread([&]() {
        j1 = pool.allocate_job([&]() {
            printf("Starting\n");
        });

        j2 = pool.allocate_job(test_job_fn, 1);

        j1->execute();

        std::thread([&]() {
            j2->execute();
            pool.free_job(j2);
        }).join();

        pool.free_all_this_thread();

        j3 = pool.allocate_job(test_job_fn, 2);
        j3->execute();
    }).join();

    REQUIRE(j2 == j3);
}