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

#include <Tasks/Tasks.hpp>
#include <string>
#include <cmath>
#include <future>
#include <Tasks/Frame.hpp>
#include <sstream>
#include <Tasks/StackAllocator.hpp>

#include <Tasks/WorkStealing/WorkStealingQueue.hpp>
#include <Tasks/WorkStealing/WorkStealingScheduler.hpp>
#include <Tasks/WorkStealing/WorkStealingJob.hpp>
#include <Tasks/WorkStealing/JobGroup.hpp>

#include "GameTest.hpp"

const uint32_t iterations = 10000;
double result = 0.0;
constexpr size_t num_jobs = 10;

uint32_t job_results[num_jobs];

constexpr uint64_t kibibytes(const uint64_t amount)
{
    return 1024 * amount;
}

constexpr uint64_t mebibytes(const uint64_t amount)
{
    return (1024 * 1024) * amount;
}

constexpr uint64_t gibibytes(const uint64_t amount)
{
    return (1024 * 1024 * 1024) * amount;
}

void test_again(const uint32_t arg1, const uint32_t arg2, const uint32_t arg3)
{
    double result = 0.0;

    for ( int i = 0; i < iterations; ++i ) {
        result += sin(arg1) + cos(arg2) + tan(arg3);
    }
}

void test_sched(const uint32_t arg)
{
    static uint32_t index = 0;
//    for ( int i = 0; i < iterations; ++i ) {
//        result += sin(arg) + cos(arg) + tan(arg);
//    }
    result += arg;
    job_results[index++] = arg;
}

enum class Tags : uint64_t {
    tag_a = 0,
    tag_b,
    tag_c,
    num_tags
};

static constexpr size_t max_threads = 5;
uint64_t* values[max_threads];

void test_heap_proc(sky::TaggedProcessHeap* tagged_heap, size_t t_id, std::atomic_bool* alive, std::mutex* mut, std::condition_variable* cv)
{
    uint64_t* ptr = nullptr;

    {
        std::unique_lock<std::mutex> lock(*mut);
        cv->wait(lock);

        for ( uint64_t k = 0; k < static_cast<uint64_t>(Tags::num_tags); ++k ) {
            tagged_heap->insert_tag(k);
        }

        std::hash<std::thread::id> hash;

        ptr = static_cast<uint64_t*>(tagged_heap->allocate(
            static_cast<const uint64_t>(Tags::tag_a), sizeof(uint64_t)
        ));

        if ( ptr != nullptr ) {
            *ptr = hash(std::this_thread::get_id());
        }

        values[t_id] = ptr;
    }

    while ( alive->load() ) {
        std::unique_lock<std::mutex> lock(*mut);
        cv->wait(lock);

        if ( ptr != nullptr ) {
            printf("Value #%zu: %llu\n", t_id, *values[t_id]);
        }
    }
}

void test_thread_heap()
{
    static constexpr size_t num_allocs = 2;

    sky::TaggedProcessHeap tagged_heap;
    tagged_heap.init(sizeof(uint64_t) * 15);

    std::vector<std::thread> threads;

    std::atomic_bool alive(true);
    std::mutex mut;
    std::condition_variable cv;

    for ( int i = 0; i < max_threads; ++i ) {
        threads.emplace_back(test_heap_proc, &tagged_heap, i, &alive, &mut, &cv);
        auto id = threads.back().get_id();
        tagged_heap.add_process(id);
    }

    for ( int j = 0; j < num_allocs; ++j ) {
        cv.notify_all();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    alive.store(false);
    cv.notify_all();

    for ( auto& t : threads ) {
        if ( t.joinable() ) {
            t.join();
        }
    }

    for ( int k = 0; k < max_threads; ++k ) {
        printf("Final #%d: %llu\n", k, *values[k]);
    }
}

int main(int argc, char** argv)
{
//
//    sky::Scheduler sched;
//    sched.init();
//
//    sky::Job jobs[num_jobs];
//
//    auto start = std::chrono::high_resolution_clock::now();
//
//    for ( int i = 0; i < num_jobs - 1; ++i ) {
//        jobs[i] = sched.create_job(test_sched, static_cast<uint32_t>(i));
//    }
//
//    jobs[num_jobs - 1] = sched.create_job(test_again, 1u, 2u, 3u);
//
//    sky::AtomicCounter counter;
//    sched.run_jobs(num_jobs, jobs, &counter);
//
//    sched.wait_for_counter(counter, 0);
//
//    auto end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
//    auto total = static_cast<double>(end.count()) / static_cast<double>(num_jobs);
//
//    printf("Time: %f\n", total);
//
//    sched.shutdown();
//
//    start = std::chrono::high_resolution_clock::now();
//
//    for ( int j = 0; j < num_jobs - 1; ++j ) {
//        test_sched(0, j);
//    }
//
//    test_again(0, 1, 2, 3);
//
//    end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
//    total = static_cast<double>(end.count()) / static_cast<double>(num_jobs);
//    printf("Time: %f\n", total);
//
//    static constexpr uint32_t num_allocations = 10000;
//
//    sky::PoolAllocator allocator;
//    allocator.init(sizeof(uint32_t), num_allocations);
//
//    start = std::chrono::high_resolution_clock::now();
//
//    for ( int j = 0; j < num_allocations; ++j ) {
//        auto data = static_cast<uint32_t*>(allocator.allocate());
//        *data = 32;
//        allocator.free(data);
//    }
//
//    end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
//    total = static_cast<double>(end.count()) / static_cast<double>(num_jobs);
//    printf("Pool allocator time: %f\n", total);
//
//    start = std::chrono::high_resolution_clock::now();
//
//    for ( int j = 0; j < num_allocations; ++j ) {
//        auto data = new uint32_t(32);
//        delete data;
//    }
//
//    end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
//    total = static_cast<double>(end.count()) / static_cast<double>(num_jobs);
//    printf("C++ new time: %f\n", total);

//    test_thread_heap();
//    std::this_thread::sleep_for(std::chrono::seconds(6));
//
//    static constexpr size_t num_allocs = 10;
//    sky::StackAllocator allocator;
//    allocator.init(sizeof(uint64_t) * num_allocs);
//
//    uint64_t* vals[num_allocs];
//
//    auto start = std::chrono::high_resolution_clock::now();
//
//    for ( int i = 0; i < 5; ++i ) {
//        for ( int j = 0; j < num_allocs; ++j ) {
//            vals[j] = static_cast<uint64_t*>(allocator.allocate(sizeof(uint64_t)));
//            *vals[j] = static_cast<uint64_t>(j);
//        }
//        allocator.clear();
//    }
//
//    auto end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
//    printf("Time: %lli\n", end.count());
//
//    start = std::chrono::high_resolution_clock::now();
//
//    for ( int i = 0; i < 5; ++i ) {
//        for ( int j = 0; j < num_allocs; ++j ) {
//            vals[j] = new uint64_t(j);
//        }
//        for ( int j = 0; j < num_allocs; ++j ) {
//            delete vals[j];
//        }
//    }
//
//    end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
//    printf("C++ new Time: %lli\n", end.count());

//    std::array<std::thread, 5> threads;
//
//    sky::TaggedProcessHeap heap;
//    heap.init(mebibytes(2));
//    heap.add_process(std::this_thread::get_id());
//    heap.insert_tag(1);
//
//    sky::Frame frame(1, 0.0, &heap);
//
//    sky::CommandBuffer<1000> buffer;
//
//    uint32_t id = 0;
//    for ( auto& t : threads ) {
//        t = std::thread([&]() {
//            heap.add_this_thread();
//            heap.insert_tag(1);
//            auto cmd = frame.emplace_data<sky::InitCommandData>(1, (++id));
//            buffer.push_command<sky::Command::Enum::init>(cmd);
//        });
//    }
//
//    for ( auto& t : threads ) {
//        if ( t.joinable() ) {
//            t.join();
//        }
//    }
//
//    sky::Command cmd{};
//    while ( buffer.pop_command(cmd) ) {
//        if ( cmd.type == sky::Command::Enum::init ) {
//            auto data = static_cast<sky::InitCommandData*>(cmd.data);
//            printf("Value: %u\n", data->id);
//        }
//    }

//    tsk::Application app{};
//    app.run();
//    app.shutdown();

    const auto num_threads = 3;

//    sky::WorkStealingScheduler sched;
//    sched.start();
//
//    for ( int i = 0; i < iterations; ++i ) {
//        sky::JobGroup jobs(&sched);
//
//        for ( int n = 0; n < num_jobs; ++n ) {
////        jobs[i] = sky::make_job(test_sched, static_cast<uint32_t>(i));
//            jobs.run_job(test_sched, static_cast<uint32_t>(n));
//        }
//
//        jobs.wait();
//
//        for ( int j = 0; j < num_jobs; ++j ) {
//            printf("Result %d: %d\n", j, job_results[j]);
//        }
//
//        printf("%f\n", result);
//        result = 0.0;
//    }
//
//    sched.shutdown();

    std::thread threads[num_threads];
    sky::WorkStealingQueue queues[num_threads];

    for ( int i = 0; i < num_threads; ++i ) {
        queues[i].resize(num_jobs);
    }

    for ( int i = 0; i < num_threads; ++i ) {
        threads[i] = std::thread([&, i, num_threads]() {
            auto job = sky::make_job(test_sched, static_cast<uint32_t>(i));
            for ( int j = 0; j < num_jobs; ++j ) {
                queues[i].push(&job);
            }

            while ( queues[(i + 1) % num_threads].size() > 0 ) {
                auto* j = queues[(i + 1) % num_threads].steal();
                if ( j != nullptr ) {
                    j->execute();
                    j->finish();
                }
            }
        });
    }

    for ( auto& t : threads ) {
        t.join();
    }


    return 0;
}