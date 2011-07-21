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

namespace gitoku
{
#define LOG_COND(expr) expr , __STRING(expr),__LINE__,__FILE__
#define _S(expr) __STRING(expr), " = ", expr, '\n'

#define LOG_POS __FILE__, ":", __LINE__
#define LOG_POS_FCT __FILE__, ':', __LINE__, " [ ", __FUNCTION__, " ] "
#define LOG_POS_PRETTY __FILE__, ':', __LINE__, " [ ", __PRETTY_FUNCTION__, " ] "

template<typename T>
void concat (std::ostringstream& ss, const T& in_arg)
{
    ss << in_arg;
}


template<typename T, typename... TArgs>
void concat (std::ostringstream& ss, const T& in_arg, const TArgs& ... in_args)
{
    ss << in_arg;
    concat (ss, in_args...);
}


template<typename... TArgs>
void error  (const char* in_module, const TArgs& ... in_args)
{
    std::ostringstream ss;
    concat(ss,in_module, ": ");
    concat(ss,in_args...);

    std::cerr << ss.str() << std::endl;
}



template<typename... TArgs>
void debug  (const char* in_module, const TArgs& ... in_args)
{
    std::ostringstream ss;
    concat(ss,in_module, ": ");
    concat(ss,in_args...);

    std::cout << ss.str() << std::endl;
}



template<typename... TArgs>
void cond_assert (bool in_condition,
                  const char* in_conditionText,
                  unsigned int in_line,
                  const char*  in_file,
                  const char* in_module,
                  const TArgs& ... in_args)
{
    if (!in_condition)
    {
        error(in_module, "assert fail  => ", in_conditionText,
                "\n","at file : ", in_file, " +", in_line,
                "\n",
                in_args...);
    }
}
}
