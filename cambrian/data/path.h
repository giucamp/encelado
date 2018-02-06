
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"

namespace cambrian
{
    using hash_t = uint32_t;

    constexpr hash_t hash(const string_view & i_source) noexcept
    {
        // http://www.cse.yorku.ca/~oz/hash.html - djb2
        uint32_t hash = 5381;
        for (char c : i_source)
        {
            hash = hash * 33 + c;
        }
        return hash;
    }

    class path
    {
      public:
    };


} // namespace cambrian
