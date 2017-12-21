//
//  TestWorker.cpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 11/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include "catch/catch.hpp"

#include <cmath>
#include <JobRocket/Worker.hpp>

constexpr size_t num_jobs = 100;
constexpr size_t num_threads = 3;
bool completed = false;

double big_calculation(double* values, const uint32_t value)
{
    constexpr size_t num_iterations = 100;
    static std::atomic<uint32_t> iteration(num_jobs);

    auto result = 0.0;
    values[value] = 0.0;
    for ( int i = 0; i < num_iterations; ++i ) {
        result += sin(value) + cos(value);
        values[value] += sin(value) + cos(value);
    }

    auto new_iteration = --iteration;
    if ( new_iteration <= 0 ) {
        completed = true;
    }

    return result;
}

TEST_CASE("Workers perform as expected", "[worker]")
{
    double values[num_jobs];
    sky::Job jobs[num_jobs];
    std::vector<sky::Worker> workers;

    workers.resize(num_threads);

    for ( auto& w : workers ) {
        w = std::move(sky::Worker(0, workers.data(), num_threads, num_jobs));
    }

    for ( int i = 0; i < num_jobs; ++i ) {
        jobs[i] = sky::make_job(big_calculation, values, static_cast<uint32_t>(i));
        workers[i % num_threads].schedule_job(&jobs[i]);
    }

    for ( auto& w : workers ) {
        w.start();
    }

    while ( !completed ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    for ( auto& w : workers ) {
        w.stop();
    }

    auto actual_result = 0.0;
    for ( double value : values ) {
        actual_result += value;
    }

    auto expected_result = 0.0;
    for ( int i = 0; i < num_jobs; i++ ) {
        expected_result += big_calculation(values, static_cast<const uint32_t>(i));
    }
    auto target_value = Approx(expected_result).epsilon(0.0001);

    REQUIRE(actual_result == target_value);
}