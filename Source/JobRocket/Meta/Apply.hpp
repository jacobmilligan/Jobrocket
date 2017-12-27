//
//  Apply.hpp
//  Tasks
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 23/11/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include "JobRocket/Meta/IndexSequence.hpp"
#include "JobRocket/Meta/Invoke.hpp"

#include <cstddef>
#include <utility>

namespace jobrocket {
namespace detail {


template <class Fn, class Tuple, size_t... Index>
constexpr decltype(auto) apply_impl(Fn&& function, Tuple&& tuple, index_sequence<Index...>)
{
    return invoke(function, std::get<Index>(std::forward<Tuple>(tuple))...);
}


} // namespace detail


template <class Fn, class Tuple>
constexpr decltype(auto) apply(Fn&& function, Tuple&& tuple)
{
    return detail::apply_impl(std::forward<Fn>(function),
                              std::forward<Tuple>(tuple),
                              make_index_sequence<
                                  std::tuple_size<
                                      typename std::remove_reference<Tuple>::type
                                  >::value
                              >{});
}


} // namespace jobrocket