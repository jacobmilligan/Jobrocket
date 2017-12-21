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
#include <JobRocket/JobGroup.hpp>
#include <JobRocket/JobPool.hpp>

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
}

void no_op_task()
{
    // no op
}

void profile_job_allocator()
{
    constexpr size_t num_objs = 1000000;

    sky::JobPool pool(1, num_objs + 1);

    Timer timer;
    timer.start();

    sky::Job job2;
    for ( int i = 0; i < num_objs; ++i ) {
        auto job = sky::make_job(no_op_task);
    }

    timer.stop();

    fmt::print("Make job: {}\n", timer.value);

    timer.start();

    sky::Job* jobptr2;
    for ( int i = 0; i < num_objs; ++i ) {
        auto* job = pool.allocate_job(no_op_task);
        pool.free_job(job);
    }

    timer.stop();

    fmt::print("Job pool: {}\n", timer.value);
}

int main(int argc, char** argv)
{
//    sched.startup(sky::Scheduler::auto_worker_count);
//
//    sky::JobGroup group(&sched);
//
//    for ( int i = 0; i <= num_jobs; ++i ) {
//        group.run(long_task, static_cast<uint32_t>(i));
//    }
//
//    group.wait_for_all();
//
//    printf("Total: %d. Expected: %d\n", total, expected);

    for ( int i = 0; i < 3; ++i ) {
        profile_job_allocator();
    }

    fmt::print("Real one\n");

    profile_job_allocator();


    return 0;
}