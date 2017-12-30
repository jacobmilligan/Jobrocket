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

    Worker(const uint32_t id, std::vector<Worker>* workers, const uint32_t max_jobs)
        : active_(false),
          queue_(max_jobs),
          rand_(1, 2),
          id_(id),
          workers_(workers),
          state_(State::ready)
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
          workers_(other.workers_),
          active_(other.active_),
          state_(other.state_)
    {}

    Worker& operator=(Worker&& other) noexcept
    {
        thread_ = std::move(other.thread_);
        queue_ = std::move(other.queue_);
        rand_ = other.rand_;
        id_ = other.id_;
        workers_ = other.workers_;
        active_ = other.active_;
        state_ = other.state_;

        return *this;
    }

    void start();

    static void pause_all();

    static void resume_all();

    void stop();

    inline void schedule_job(Job* job)
    {
        queue_.push(job);
    }

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

    Job* get_next_job();

private:
    std::thread thread_;
    detail::FixedWorkStealingQueue queue_;
    detail::xoroshiro128 rand_{1, 2};

    uint32_t id_{0};
    std::vector<Worker>* workers_{nullptr};
    bool active_{false};
    State state_;

    static std::condition_variable cv_;
    static std::mutex mutex_;
    static bool paused_;

    void main_proc();
};


} // namespace jobrocket