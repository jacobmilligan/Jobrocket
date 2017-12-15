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
#include "JobRocket/AtomicCounter.hpp"

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

    constexpr uint32_t size()
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

    AtomicCounter* group_counter;
    State state;

    static constexpr size_t data_size = sizeof(state) + sizeof(group_counter);

    uint8_t function[64 - data_size]{};

    Job()
        : state(State::unknown), group_counter(nullptr)
    {}

    Job(size_t size, void* job_function)
        : state(State::unknown), group_counter(nullptr)
    {
        memcpy(function, job_function, size);
        state = State::ready;
    }

    void execute()
    {
        state = State::running;
        reinterpret_cast<JobFunctionBase*>(function)->execute();
        state = State::completed;
        
        if ( group_counter != nullptr ) {
            group_counter->decrement();
        }
    }
};


template <typename Fn, typename... Args>
Job make_job(Fn function, Args&&... args)
{
    auto data = JobFunction<Fn, Args...>(function, std::forward<Args>(args)...);
    return Job(data.size(), &data);
}


}