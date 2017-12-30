//
//  Worker.cpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 30/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include "JobRocket/Worker.hpp"
#include "JobRocket/JobPool.hpp"

namespace jobrocket {


std::condition_variable Worker::cv_;
std::mutex Worker::mutex_;
bool Worker::paused_ = false;


void Worker::start()
{
    active_ = true;
    thread_ = std::thread(&Worker::main_proc, this);

    while ( true ) {
        if ( state_ == Worker::State::running ) {
            break;
        }
    }
}

void Worker::stop()
{
    active_ = false;
    if ( thread_.joinable() ) {
        thread_.join();
    }

    state_ = State::terminated;
}

void Worker::pause_all()
{
    paused_ = true;
}

void Worker::resume_all()
{
    paused_ = false;
    cv_.notify_one();
}

Job* Worker::get_next_job()
{
    Job* next_job = nullptr;

    auto pop_success = queue_.pop(next_job);
    if ( pop_success && next_job != nullptr && next_job->state == jobrocket::Job::State::ready ) {
        return next_job;
    }

    auto rand_worker = rand_.next() % workers_->size();
    if ( rand_worker != id_ ) {
        auto steal_success = (*workers_)[rand_worker].queue_.steal(next_job);
        if ( steal_success && next_job != nullptr && next_job->state == jobrocket::Job::State::ready ) {
            return next_job;
        }
    }

    return nullptr;
}

void Worker::main_proc()
{
    jobrocket::Job* job = nullptr;
    state_ = State::running;

    while ( active_ ) {
        while ( paused_ ) {
            state_ = State::waiting;
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [&]() { return !paused_; });
            state_ = State::running;
        }

        job = get_next_job();
        if ( job != nullptr ) {
            job->execute();
            if ( job->source_pool != nullptr ) {
                job->source_pool->free_job(job);
            }
        }
    }
}


} // namespace jobrocket