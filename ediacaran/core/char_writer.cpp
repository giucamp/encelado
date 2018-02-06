
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "char_writer.h"
#include <cstdio>
#include <inttypes.h>
#include <limits>
#include <stdexcept>

namespace edi
{
    char_writer & operator<<(char_writer & i_dest, float i_number)
    {
        char temp[64];
        snprintf(
          temp, sizeof(temp) - 1, "%.*g", std::numeric_limits<float>::max_digits10, i_number);
        i_dest << temp;
        return i_dest;
    }

    char_writer & operator<<(char_writer & i_dest, double i_number)
    {
        char temp[128];
        snprintf(
          temp, sizeof(temp) - 1, "%.*g", std::numeric_limits<double>::max_digits10, i_number);
        i_dest << temp;
        return i_dest;
    }

    char_writer & operator<<(char_writer & i_dest, long double i_number)
    {
        char temp[128];
        snprintf(
          temp,
          sizeof(temp) - 1,
          "%.*Lg",
          std::numeric_limits<long double>::max_digits10,
          i_number);
        i_dest << temp;
        return i_dest;
    }
}
