//
//  Error.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 27/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

namespace jobrocket {
namespace detail {

void print_error(const char* source, const char* what);
void print_error(const char* source, const char* what, const char* help);

} // namespace detail
} // namespace jobrocket