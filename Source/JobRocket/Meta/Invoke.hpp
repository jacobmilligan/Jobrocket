//
//  Invoke.hpp
//  Tasks
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 23/11/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include <type_traits>
#include <functional>

namespace sky {

// Based on: https://stackoverflow.com/questions/38288042/c11-14-invoke-workaround

template<typename Fn, typename... Args,
    std::enable_if_t<std::is_member_pointer< std::decay_t<Fn> >{}, int> = 0>
constexpr decltype(auto) invoke(Fn&& f, Args&& ... args)
    noexcept(noexcept( std::mem_fn(f)(std::forward<Args>(args)...) ))
{
    return std::mem_fn(f)(std::forward<Args>(args)...);
}

template<typename Fn, typename... Args,
    std::enable_if_t<!std::is_member_pointer< std::decay_t<Fn>>{}, int > = 0>
constexpr decltype(auto) invoke(Fn&& f, Args&& ... args)
    noexcept(noexcept( std::forward<Fn>(f)(std::forward<Args>(args)...) ))
{
    return std::forward<Fn>(f)(std::forward<Args>(args)...);
}



}