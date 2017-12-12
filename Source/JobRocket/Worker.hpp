//
//  Worker.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 11/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "JobRocket/StaticWorkStealingQueue.hpp"
#include "JobRocket/RNG.hpp"

#include <thread>
#include <cassert>

namespace sky {


class Worker {
public:
    Worker() = default;

    Worker(const u32 id, Worker* workers, const u32 num_workers, const u32 max_jobs)
        : active_(false),
          queue_(max_jobs),
          rand_(1, 2),
          id_(id),
          num_workers_(num_workers),
          workers_(workers)
    {}

    ~Worker()
    {
        stop();
    }

    Worker(const Worker& other) = delete;

    Worker& operator=(const Worker&) = delete;

    Worker(Worker&& other) noexcept
        : thread_(std::move(other.thread_)),
          queue_(std::move(other.queue_)),
          rand_(other.rand_),
          id_(other.id_),
          num_workers_(other.num_workers_),
          workers_(other.workers_),
          active_(other.active_)
    {}

    Worker& operator=(Worker&& other) noexcept
    {
        thread_ = std::move(other.thread_);
        queue_ = std::move(other.queue_);
        rand_ = other.rand_;
        id_ = other.id_;
        num_workers_ = other.num_workers_;
        workers_ = other.workers_;
        active_ = other.active_;

        return *this;
    }

    void start()
    {
        active_ = true;
        thread_ = std::thread(&Worker::main_proc, this);
    }

    void stop()
    {
        active_ = false;
        if ( thread_.joinable() ) {
            thread_.join();
        }
    }

    void schedule_job(const Job& job)
    {
        queue_.push(job);
    }

private:
    std::thread thread_;
    StaticWorkStealingQueue queue_;
    xoroshiro128 rand_{1, 2};

    std::condition_variable cv_;
    std::mutex mutex_;

    u32 id_{0};
    u32 num_workers_{0};
    Worker* workers_{nullptr};
    bool active_{false};

    void main_proc()
    {
        sky::Job* job = nullptr;

        while ( active_ ) {
            job = get_next_job();
            if ( job != nullptr ) {
                job->execute();
            }
        }
    }

    Job* get_next_job()
    {
        Job* next_job = nullptr;

        auto pop_success = queue_.pop(&next_job);
        if ( pop_success && next_job->state == sky::Job::State::ready ) {
            return next_job;
        }

        auto rand_worker = rand_.next() % num_workers_;
        if ( rand_worker != id_ ) {
            auto steal_success = workers_[rand_worker].queue_.steal(&next_job);
            if ( steal_success && next_job->state == sky::Job::State::ready ) {
                return next_job;
            }
        }

        return nullptr;
    }
};


} // namespace sky