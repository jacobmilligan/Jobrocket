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

#include "JobRocket/Meta/Apply.hpp"
#include "JobRocket/NumericTypes.hpp"

#include <tuple>

namespace sky {


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

    constexpr u32 size()
    {
        return sizeof(JobFunction<Fn, Args...>);
    }

    void execute() override
    {
        apply(function_ptr, arg_tuple);
    }
};

struct Job {
    enum class State {
        unknown,
        ready,
        running,
        completed
    };

    State state;
    u8 function[64 - sizeof(state)]{};

    Job()
        : state(State::unknown)
    {}

    Job(size_t size, void* job_function)
        : state(State::unknown)
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
Job make_job(Fn function, Args&&... args)
{
    auto data = JobFunction<Fn, Args...>(function, std::forward<Args>(args)...);
    return Job(data.size(), &data);
}


}