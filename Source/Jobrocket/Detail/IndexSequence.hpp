//
//  index_sequence.hpp
//  Tasks
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 23/11/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include <cstddef>

namespace jobrocket {
namespace detail {

// Based on: https://stackoverflow.com/questions/17424477/implementation-c14-make-integer-sequence

template <size_t... I>
struct index_sequence {
    using type = index_sequence;
    using value_type = size_t;

    static constexpr size_t size()
    {
        return sizeof...(I);
    }
};

template <typename SequenceA, typename SequenceB>
struct concat_sequence;

template <size_t...I1, size_t... I2>
struct concat_sequence< index_sequence<I1...>, index_sequence<I2...> >
    : public index_sequence< I1..., (sizeof ... (I1) + I2)... >
{};

template <size_t I>
struct make_index_sequence : public concat_sequence
    <
        typename make_index_sequence<I / 2>::type,
        typename make_index_sequence<I - I / 2>::type
    >
{};

template <>
struct make_index_sequence<0> : public index_sequence<>
{};

template <>
struct make_index_sequence<1> : public index_sequence<0>
{};

template <typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;



} // namespace detail
} // namespace jobrocket