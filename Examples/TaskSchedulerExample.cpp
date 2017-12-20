//
//  JobSchedulerExample.cpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 9/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include <JobRocket/FixedWorkStealingQueue.hpp>
#include <random>
#include <thread>

uint64_t random_index()
{
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

constexpr size_t num_threads = 4;
constexpr size_t num_jobs = 100;
double confirmations[num_jobs];

double big_calculation(const uint32_t value)
{
    constexpr size_t num_iterations = 10000;

    auto result = 0.0;
    confirmations[value] = 0.0;
    for ( int i = 0; i < num_iterations; ++i ) {
        result += sin(value) + cos(value);
        confirmations[value] += sin(value) + cos(value);
    }
    return result;
}

void parallel_for(std::atomic_uint* job_count, std::thread* threads, sky::FixedWorkStealingQueue* queues,
                  std::atomic_bool* active, const int value)
{
    if ( value > 0 ) {
        confirmations[value] = big_calculation(value);
        auto thread_id = std::this_thread::get_id();
        for ( int t = 0; t < num_threads; ++t ) {
            if ( thread_id == threads[t].get_id() ) {
                auto job = sky::make_job(parallel_for, job_count, threads, queues, active, value - 1);
                queues[t].push(job);
                job_count->fetch_add(1);
            }
        }
    } else {
        active->store(false);
    }
}

void reset_values()
{
    for ( auto& v : confirmations ) {
        v = 0.0;
    }
}

void queue_example()
{
    std::atomic_bool active(false);
    std::thread threads[num_threads];
    std::vector<sky::FixedWorkStealingQueue> queues;

    std::atomic_uint job_count(num_jobs);

    auto worker_proc = [&](const size_t index) {
        sky::Job* job = nullptr;
        while ( active.load() ) {
            auto pop_success = queues[index].pop(&job);
            if ( pop_success && job->state == sky::Job::State::ready ) {
                job->execute();
                --job_count;
            } else {
                auto rand_worker = random_index() % num_threads;
                if ( rand_worker != index ) {
                    auto steal_success = queues[rand_worker].steal(&job);
                    if ( steal_success && job->state == sky::Job::State::ready ) {
                        job->execute();
                        --job_count;
                    }
                }
            }

            if ( job_count.load() <= 0  ) {
                active.store(false);
            }
        }
    };

    for ( int i = 0; i < num_threads; ++i ) {
        queues.emplace_back(num_jobs);
    }

    active.store(true);
    auto start = std::chrono::high_resolution_clock::now();

    for ( int t = 0; t < num_jobs; ++t ) {
        auto job = sky::make_job(big_calculation, static_cast<uint32_t>(t));
        queues[t % num_threads].push(job);
    }

    for ( int i = 0; i < num_threads; ++i ) {
        threads[i] = std::thread(worker_proc, i);
    }

    for ( auto& t : threads ) {
        if ( t.joinable() ) {
            t.join();
        }
    }

    auto actual_result = 0.0;
    for ( int j = 0; j < num_jobs; j++ ) {
        actual_result += confirmations[j];
    }

    auto end = std::chrono::high_resolution_clock::now() - start;
    printf("Time: %lld\n", end.count());

    start = std::chrono::high_resolution_clock::now();

    auto expected_result = 0.0;
    for ( int k = 0; k < num_jobs; k++ ) {
        expected_result += big_calculation(k);
    }

    end = std::chrono::high_resolution_clock::now() - start;
    printf("Time: %lld\n", end.count());

    printf("Expected: %lf. Result: %lf\n", expected_result, actual_result);
}

int main(int argc, char** argv)
{
    queue_example();
}