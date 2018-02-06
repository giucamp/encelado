
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ediacaran/reflection/reflection.h"
#include <memory>

namespace edi
{
    template <typename... PARAMS> struct non_intrusive_reflection<std::allocator<PARAMS...>>
    {
        static constexpr auto reflect()
        {
            return make_class<std::allocator<PARAMS...>>("std::allocator");
        }
    };

} // namespace edi
