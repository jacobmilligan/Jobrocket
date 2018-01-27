//
//  Job.hpp
//  Jobrocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 9/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "Jobrocket/Detail/Apply.hpp"
#include "Jobrocket/Detail/AtomicCounter.hpp"

#include <tuple>
#include <thread>

namespace jobrocket {

class JobPool;

/// Abstract base class used to call the function regardless of arguments
struct JobFunctionBase {
    virtual void execute() = 0;
};

/// Holds a jobs function pointer and argument tuple, applying the
/// arguments to the function and calling it when `execute()` is called.
template <typename Fn, typename... Args>
struct JobFunction : JobFunctionBase {
    Fn function_ptr;
    std::tuple<Args...> arg_tuple;

    explicit JobFunction(Fn function, Args&&... args)
        : function_ptr(function),
          arg_tuple(std::tuple<Args...>(args...))
    {}

    constexpr uint32_t size()
    {
        return sizeof(JobFunction<Fn, Args...>);
    }

    void execute() override
    {
        detail::apply(function_ptr, arg_tuple);
    }
};

/// A job is a function pointer and series of arguments to the function as well as providing a link
/// back to its thread-local allocating pool and `JobGroup` counter.
struct Job {
    /// Represents the state of a job at any given time
    enum class State {
        unknown,
        ready,
        running,
        completed
    };

    /// The source pool from which the job was allocated and should be free'd from
    JobPool* source_pool;
    /// The jobs current running state
    State state;
    /// Pointer to the counter belonging to a `JobGroup` if the job belongs to one
    detail::AtomicCounter* group_counter;
    /// The id of the worker that the job was allocated from
    uint32_t worker_alloc;

    /// Size of all members, used to offset the function buffer
    static constexpr size_t data_size = sizeof(source_pool) + sizeof(state) +
        sizeof(group_counter) + sizeof(worker_alloc);

    /// Buffer holding the `JobFunction` and associated arguments
    uint8_t function[64 - data_size]{};

    Job()
        : state(State::unknown),
          group_counter(nullptr),
          worker_alloc(0),
          source_pool(nullptr)
    {}

    Job(const size_t size, void* job_function, const uint32_t allocating_worker)
        : state(State::unknown),
          group_counter(nullptr),
          worker_alloc(allocating_worker),
          source_pool(nullptr)
    {
        memcpy(function, job_function, size);
        state = State::ready;
    }

    /// Runs the jobs function
    void execute()
    {
        state = State::running;
        reinterpret_cast<JobFunctionBase*>(function)->execute();
        state = State::completed;
    }
};


/// Makes a new job on the stack without a `JobPool`
template <typename Fn, typename... Args>
Job make_unmanaged_job(Fn function, Args&& ... args)
{
    auto data = JobFunction<Fn, Args...>(function, std::forward<Args>(args)...);
    return Job(data.size(), &data, 0);
}

inline bool is_ready(Job* job)
{
    return job != nullptr && job->state == jobrocket::Job::State::ready;
}


}