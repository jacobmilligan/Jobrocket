//
//  Job.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 9/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "JobRocket/Detail/Apply.hpp"
#include "JobRocket/Detail/AtomicCounter.hpp"

#include <tuple>
#include <thread>

namespace jobrocket {

class JobPool;

struct JobFunctionBase {
    virtual void execute() = 0;
};

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

struct Job {
    enum class State {
        unknown,
        ready,
        running,
        completed
    };

    JobPool* source_pool;
    State state;
    detail::AtomicCounter* group_counter;
    uint32_t worker_alloc;

    static constexpr size_t data_size = sizeof(source_pool) + sizeof(state) +
        sizeof(group_counter) + sizeof(worker_alloc);

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

    void execute()
    {
        state = State::running;
        reinterpret_cast<JobFunctionBase*>(function)->execute();
        state = State::completed;
    }
};


template <typename Fn, typename... Args>
Job make_unmanaged_job(Fn function, Args&& ... args)
{
    auto data = JobFunction<Fn, Args...>(function, std::forward<Args>(args)...);
    return Job(data.size(), &data, 0);
}


}