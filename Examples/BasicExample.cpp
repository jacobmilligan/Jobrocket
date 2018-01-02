//
//  BasicExample.cpp
//  JobRocket Examples
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 12/11/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include <JobRocket/JobRocket.hpp>
#include <JobRocket/JobGroup.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

struct Timer {
    double value{0.0};

    void start()
    {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void stop()
    {
        auto end = std::chrono::high_resolution_clock::now();
        value = std::chrono::duration<double, std::milli>(end - start_time).count();
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

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

void no_op_job()
{
    // no op
}

static uint32_t countdown = (num_jobs * num_jobs) + num_jobs;

void subjob(const uint32_t parent, const uint32_t job_num)
{
    fmt::print("\tParent: {0} | Subtask: {1}\n", parent, job_num);
    --countdown;
}

void print_job(const char* msg, const uint32_t index)
{
    fmt::print("Message: '{0}'  | Index: {1}\n", msg, index);
    jobrocket::JobGroup group;

    for ( int i = 0; i < num_jobs; ++i ) {
        group.run(subjob, index, uint32_t(i));
    }

    group.wait_for_all();
    --countdown;
}

int main(int argc, char** argv)
{
    Timer timer;
    timer.start();

    jobrocket::startup(jobrocket::Scheduler::auto_thread_count, 1);

    fmt::print("Start value ({0} workers): {1}\n", jobrocket::current_scheduler()->num_workers(), countdown);

    jobrocket::JobGroup group;
    for ( int i = 0; i < num_jobs; ++i ) {
        group.run(print_job, "Job Group", uint32_t(i));
    }

    group.wait_for_all();

    fmt::print("Finish value: {0}\n\n", countdown);

    jobrocket::make_job_and_wait(print_job, "Single job wait", 0);
    timer.stop();

    fmt::print("Time: {}\n", timer.value);
    fmt::print("Done\n\n");

    auto job = jobrocket::make_job(print_job, "Manual job run and wait", 0);
    jobrocket::run(job);
    jobrocket::wait(job);
    fmt::print("Done\n\n");

    auto t = std::thread([&]() {
        jobrocket::register_main_thread();
        while ( active ) {
            jobrocket::make_job_and_wait([]() {
                fmt::print("hey it's me ur main thread\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            });
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));

    fmt::print("Shutting down secondary main thread\n");
    active = false;
    t.join();

    jobrocket::shutdown();


    return 0;
}