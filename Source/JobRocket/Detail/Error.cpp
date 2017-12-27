//
//  Error.cpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 27/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include "JobRocket/Detail/Error.hpp"

#include <fmt/format.h>

namespace jobrocket {
namespace detail {


void print_error(const char* source, const char* what)
{
    fmt::print(stderr, "{0} error: {1}\n", source, what);
}

void print_error(const char* source, const char* what, const char* help)
{
    static const char* arrow = "->";

    auto len = static_cast<int>(strlen(source) - strlen(arrow) + 2);
    fmt::print(stderr, "{0} error: {1}\n{2:>{3}} help: {4}\n", source, what, arrow, len, help);
}


} // namespace detail
} // namespace jobrocket