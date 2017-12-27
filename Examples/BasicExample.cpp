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
#include <JobRocket/Scheduler.hpp>

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

void print_job(const char* msg, const uint32_t num_workers)
{
    fmt::print("Message: {0}\nNum workers: {1}", msg, num_workers);
}

int main(int argc, char** argv)
{

    jobrocket::startup();

    auto* job = jobrocket::make_job(print_job, "Hey", jobrocket::current_scheduler()->num_workers());
    job->execute();

    jobrocket::shutdown();


    return 0;
}