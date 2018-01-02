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

#include "JobRocket/Detail/FixedWorkStealingQueue.hpp"
#include "JobRocket/Detail/RNG.hpp"

#include <thread>
#include <cassert>

namespace jobrocket {


class Worker {
public:
    enum class State {
        ready,
        running,
        waiting,
        terminated
    };

    Worker() = default;

    Worker(const uint32_t id, std::vector<Worker>* workers, const uint32_t max_jobs,
           std::mutex* parent_mutex, std::condition_variable* parent_cv,
           detail::AtomicCounter* parent_counter)
        : queue_(max_jobs),
          rand_(1, 2),
          id_(id),
          workers_(workers),
          state_(State::ready),
          mutex_(parent_mutex),
          cv_(parent_cv),
          active_jobs_(parent_counter)
    {}

    ~Worker()
    {
        terminate();
        join();
    }

    Worker(const Worker& other) = delete;

    Worker& operator=(const Worker&) = delete;

    Worker(Worker&& other) noexcept
        : thread_(std::move(other.thread_)),
          queue_(std::move(other.queue_)),
          rand_(other.rand_),
          id_(other.id_),
          workers_(other.workers_),
          state_(other.state_),
          mutex_(other.mutex_),
          cv_(other.cv_),
          active_jobs_(other.active_jobs_)
    {}

    Worker& operator=(Worker&& other) noexcept
    {
        thread_ = std::move(other.thread_);
        queue_ = std::move(other.queue_);
        rand_ = other.rand_;
        id_ = other.id_;
        workers_ = other.workers_;
        state_ = other.state_;
        mutex_ = other.mutex_;
        cv_ = other.cv_;
        active_jobs_ = other.active_jobs_;

        return *this;
    }

    void start();

    void terminate();

    void join();

    void schedule_job(Job* job);

    inline bool owns_this_thread()
    {
        return thread_.get_id() == std::this_thread::get_id();
    }

    inline State state()
    {
        return state_;
    }

    inline uint32_t id()
    {
        return id_;
    }

    bool try_run_job();
private:
    std::thread thread_;
    detail::FixedWorkStealingQueue queue_;
    detail::xoroshiro128 rand_{1, 2};

    uint32_t id_{0};
    std::vector<Worker>* workers_{nullptr};
    State state_;

    std::condition_variable* cv_;
    std::mutex* mutex_;
    detail::AtomicCounter* active_jobs_;

    void main_proc();
    Job* get_next_job();
};


} // namespace jobrocket