//
//  TestQueue.cpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 9/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include "catch/catch.hpp"
#include <JobRocket/Detail/FixedWorkStealingQueue.hpp>

#include <thread>
#include <random>

class QueueTestFixture {
public:
    QueueTestFixture()
        : active_(false)
    {
        for ( int i = 0; i < num_threads; ++i ) {
            queues.emplace_back(num_jobs);
        }
    }

    ~QueueTestFixture()
    {
        active_.store(false);

        for ( auto& t : threads_ ) {
            if ( t.joinable() ) {
                t.join();
            }
        }
    }

    void start()
    {
        active_.store(true);

        for ( int i = 0; i < num_threads; ++i ) {
            threads_[i] = std::thread(&QueueTestFixture::thread_proc, this, i);
        }

        for ( auto& t : threads_ ) {
            if ( t.joinable() ) {
                t.join();
            }
        }
    }

    static constexpr size_t num_jobs = 100;
    static constexpr size_t num_threads = 4;

    std::vector<jobrocket::detail::FixedWorkStealingQueue> queues;
    std::atomic_uint job_count{num_jobs};
private:
    std::atomic_bool active_{false};
    std::thread threads_[num_threads];

    uint64_t random_index()
    {
        // xorshift+
        static std::random_device rd;
        static uint64_t s[2] = {
            (uint64_t(rd()) << 32) ^ (rd()),
            (uint64_t(rd()) << 32) ^ (rd())
        };

        auto x = s[0];
        const auto y = s[1];

        s[0] = y;
        x ^= x << 23;
        s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
        return s[1] + y;
    }

    void thread_proc(const uint32_t index)
    {
        jobrocket::Job* job = nullptr;
        while ( active_.load() ) {
            auto pop_success = queues[index].pop(job);
            if ( pop_success && job->state == jobrocket::Job::State::ready ) {
                job->execute();
                --job_count;
            } else {
                auto rand_worker = random_index() % num_threads;
                if ( rand_worker != index ) {
                    auto steal_success = queues[rand_worker].steal(job);
                    if ( steal_success && job->state == jobrocket::Job::State::ready ) {
                        job->execute();
                        --job_count;
                    }
                }
            }

            if ( job_count.load() <= 0  ) {
                active_.store(false);
            }
        }
    }
};

constexpr size_t QueueTestFixture::num_jobs;
constexpr size_t QueueTestFixture::num_threads;

double big_calculation(double* values, const uint32_t value)
{
    constexpr size_t num_iterations = 100;

    auto result = 0.0;
    values[value] = 0.0;
    for ( int i = 0; i < num_iterations; ++i ) {
        result += sin(value) + cos(value);
        values[value] += sin(value) + cos(value);
    }
    return result;
}

TEST_CASE_METHOD(QueueTestFixture, "Queue works for basic numeric operations", "[queue]")
{
    double values[num_jobs];
    jobrocket::Job jobs[num_jobs];

    for ( int i = 0; i < num_jobs; ++i ) {
        jobs[i] = jobrocket::make_unmanaged_job(big_calculation, values, static_cast<uint32_t>(i));
        queues[i % num_threads].push(&jobs[i]);
    }

    start();

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