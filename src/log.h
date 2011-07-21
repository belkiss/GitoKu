/*
 * Copyright (C) 2011  Luc Bertrand <grumpy@cacou.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#pragma once
#include <iostream>
#include <sstream>


#if __GNUC__ == 4 && __GNUC_MINOR__ >= 6
#else
    #define nullptr 0
#endif

namespace gitoku
{

template<typename... TArgs>
inline void debug  (const char* in_module, const TArgs& ... in_args);

template<typename... TArgs>
inline void error  (const char* in_module, const TArgs& ... in_args);


template<typename... TArgs>
void cond_assert (bool in_condition,
                  const char* in_conditionText,
                  unsigned int in_line,
                  const char*  in_file,
                  const char* in_module,
                  const TArgs& ... in_args);
}

#include "log.hpp"